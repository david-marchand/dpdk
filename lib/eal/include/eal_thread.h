/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef EAL_THREAD_H
#define EAL_THREAD_H

#include <pthread.h>

#include <rte_compat.h>
#include <rte_thread.h>

__rte_internal
int
rte_thread_create_internal_control(rte_thread_t *id, const char *name,
		rte_thread_func func, void *arg);

__rte_internal
void
rte_thread_set_prefixed_name(rte_thread_t id, const char *name);

__rte_internal
void rte_thread_mutex_init_shared(pthread_mutex_t *mutex);

#endif /* EAL_THREAD_H */
