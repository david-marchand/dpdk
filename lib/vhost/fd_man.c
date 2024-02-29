/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <rte_common.h>
#include <rte_log.h>

#include "fd_man.h"

RTE_LOG_REGISTER_SUFFIX(vhost_fdset_logtype, fdset, INFO);
#define RTE_LOGTYPE_VHOST_FDMAN vhost_fdset_logtype
#define VHOST_FDMAN_LOG(level, ...) \
	RTE_LOG_LINE(level, VHOST_FDMAN, "" __VA_ARGS__)

void
fdset_init(struct fdset *pfdset)
{
	int i;

	if (pfdset == NULL)
		return;

	for (i = 0; i < (int)RTE_DIM(pfdset->fd); i++) {
		pfdset->fd[i].fd = -1;
		pfdset->fd[i].dat = NULL;
	}
	LIST_INIT(&pfdset->fdlist);
}

/**
 * Register the fd in the fdset with read/write handler and context.
 */
int
fdset_add(struct fdset *pfdset, int fd, fd_cb rcb, fd_cb wcb, void *dat)
{
	struct fdentry *pfdentry;
	struct epoll_event ev;

	if (pfdset == NULL || fd == -1)
		return -1;

	pthread_mutex_lock(&pfdset->fd_mutex);
	if (pfdset->next_free_idx >= (int)RTE_DIM(pfdset->fd)) {
		pthread_mutex_unlock(&pfdset->fd_mutex);
		return -2;
	}

	pfdentry = &pfdset->fd[pfdset->next_free_idx];
	pfdentry->fd  = fd;
	pfdentry->rcb = rcb;
	pfdentry->wcb = wcb;
	pfdentry->dat = dat;

	LIST_INSERT_HEAD(&pfdset->fdlist, pfdentry, next);

	/* Find next free slot */
	pfdset->next_free_idx++;
	for (; pfdset->next_free_idx < (int)RTE_DIM(pfdset->fd); pfdset->next_free_idx++) {
		if (pfdset->fd[pfdset->next_free_idx].fd != -1)
			continue;
		break;
	}
	pthread_mutex_unlock(&pfdset->fd_mutex);

	ev.events = EPOLLERR;
	ev.events |= rcb ? EPOLLIN : 0;
	ev.events |= wcb ? EPOLLOUT : 0;
	ev.data.fd = fd;

	if (epoll_ctl(pfdset->epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		VHOST_FDMAN_LOG(ERR, "could not add %d fd to %d epfd: %s",
			fd, pfdset->epfd, strerror(errno));

	return 0;
}

static struct fdentry *
fdset_find_entry_locked(struct fdset *pfdset, int fd)
{
	struct fdentry *pfdentry;

	LIST_FOREACH(pfdentry, &pfdset->fdlist, next) {
		if (pfdentry->fd != fd)
			continue;
		return pfdentry;
	}

	return NULL;
}

static void
fdset_del_locked(struct fdset *pfdset, struct fdentry *pfdentry)
{
	int entry_idx;

	if (epoll_ctl(pfdset->epfd, EPOLL_CTL_DEL, pfdentry->fd, NULL) == -1)
		VHOST_FDMAN_LOG(ERR, "could not remove %d fd from %d epfd: %s",
			pfdentry->fd, pfdset->epfd, strerror(errno));

	pfdentry->fd = -1;
	pfdentry->rcb = pfdentry->wcb = NULL;
	pfdentry->dat = NULL;
	entry_idx = pfdentry - pfdset->fd;
	if (entry_idx < pfdset->next_free_idx)
		pfdset->next_free_idx = entry_idx;
	LIST_REMOVE(pfdentry, next);
}

void
fdset_del(struct fdset *pfdset, int fd)
{
	struct fdentry *pfdentry;

	if (pfdset == NULL || fd == -1)
		return;

	do {
		pthread_mutex_lock(&pfdset->fd_mutex);
		pfdentry = fdset_find_entry_locked(pfdset, fd);
		if (pfdentry != NULL && pfdentry->busy == 0) {
			fdset_del_locked(pfdset, pfdentry);
			pfdentry = NULL;
		}
		pthread_mutex_unlock(&pfdset->fd_mutex);
	} while (pfdentry != NULL);
}

/**
 *  Unregister the fd from the fdset.
 *
 *  If parameters are invalid, return directly -2.
 *  And check whether fd is busy, if yes, return -1.
 *  Otherwise, try to delete the fd from fdset and
 *  return true.
 */
int
fdset_try_del(struct fdset *pfdset, int fd)
{
	struct fdentry *pfdentry;

	if (pfdset == NULL || fd == -1)
		return -2;

	pthread_mutex_lock(&pfdset->fd_mutex);
	pfdentry = fdset_find_entry_locked(pfdset, fd);
	if (pfdentry != NULL && pfdentry->busy != 0) {
		pthread_mutex_unlock(&pfdset->fd_mutex);
		return -1;
	}

	if (pfdentry != NULL)
		fdset_del_locked(pfdset, pfdentry);

	pthread_mutex_unlock(&pfdset->fd_mutex);
	return 0;
}

/**
 * This functions runs in infinite blocking loop until there is no fd in
 * pfdset. It calls corresponding r/w handler if there is event on the fd.
 *
 * Before the callback is called, we set the flag to busy status; If other
 * thread(now rte_vhost_driver_unregister) calls fdset_del concurrently, it
 * will wait until the flag is reset to zero(which indicates the callback is
 * finished), then it could free the context after fdset_del.
 */
uint32_t
fdset_event_dispatch(void *arg)
{
	int i;
	fd_cb rcb, wcb;
	void *dat;
	int fd, numfds;
	int remove1, remove2;
	struct fdset *pfdset = arg;

	if (pfdset == NULL)
		return 0;

	while (1) {
		struct epoll_event events[MAX_FDS];
		struct fdentry *pfdentry;

		numfds = epoll_wait(pfdset->epfd, events, RTE_DIM(events), 1000);
		if (numfds < 0)
			continue;

		for (i = 0; i < numfds; i++) {
			pthread_mutex_lock(&pfdset->fd_mutex);

			fd = events[i].data.fd;
			pfdentry = fdset_find_entry_locked(pfdset, fd);
			if (pfdentry == NULL) {
				pthread_mutex_unlock(&pfdset->fd_mutex);
				continue;
			}

			remove1 = remove2 = 0;

			rcb = pfdentry->rcb;
			wcb = pfdentry->wcb;
			dat = pfdentry->dat;
			pfdentry->busy = 1;

			pthread_mutex_unlock(&pfdset->fd_mutex);

			if (rcb && events[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP))
				rcb(fd, dat, &remove1);
			if (wcb && events[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP))
				wcb(fd, dat, &remove2);
			pfdentry->busy = 0;
			/*
			 * fdset_del needs to check busy flag.
			 * We don't allow fdset_del to be called in callback
			 * directly.
			 */
			/*
			 * A concurrent fdset_del may have been waiting for the
			 * fdentry not to be busy, so we can't call
			 * fdset_del_locked().
			 */
			if (remove1 || remove2)
				fdset_del(pfdset, fd);
		}
	}

	return 0;
}

static void
fdset_pipe_read_cb(int readfd, void *dat __rte_unused,
		   int *remove __rte_unused)
{
	char charbuf[16];
	int r = read(readfd, charbuf, sizeof(charbuf));
	/*
	 * Just an optimization, we don't care if read() failed
	 * so ignore explicitly its return value to make the
	 * compiler happy
	 */
	RTE_SET_USED(r);
}

void
fdset_pipe_uninit(struct fdset *fdset)
{
	fdset_del(fdset, fdset->u.readfd);
	close(fdset->u.readfd);
	close(fdset->u.writefd);
	close(fdset->epfd);
}

int
fdset_pipe_init(struct fdset *fdset)
{
	int ret;

	fdset->epfd = epoll_create(255);
	if (fdset->epfd < 0) {
		VHOST_FDMAN_LOG(ERR,
			"failed to create epoll for vhost fdset");
		return -1;
	}
	if (pipe(fdset->u.pipefd) < 0) {
		VHOST_FDMAN_LOG(ERR,
			"failed to create pipe for vhost fdset");
		close(fdset->epfd);
		return -1;
	}

	ret = fdset_add(fdset, fdset->u.readfd,
			fdset_pipe_read_cb, NULL, NULL);

	if (ret < 0) {
		VHOST_FDMAN_LOG(ERR,
			"failed to add pipe readfd %d into vhost server fdset",
			fdset->u.readfd);

		fdset_pipe_uninit(fdset);
		return -1;
	}

	return 0;
}

void
fdset_pipe_notify(struct fdset *fdset)
{
	int r = write(fdset->u.writefd, "1", 1);
	/*
	 * Just an optimization, we don't care if write() failed
	 * so ignore explicitly its return value to make the
	 * compiler happy
	 */
	RTE_SET_USED(r);

}
