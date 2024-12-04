/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2024 Red Hat, Inc.
 */

#include <stdbool.h>
#include <sys/queue.h>

#include <rte_common.h>
#include <rte_os.h>

#include "eal_private.h"

static bool init_called;

static TAILQ_HEAD(, rte_init_deferred) init_list = TAILQ_HEAD_INITIALIZER(init_list);

void
rte_init_deferred_register(struct rte_init_deferred *i)
{
	struct rte_init_deferred *p;

	if (init_called) {
		/*
	 	 * This code is being executed after rte_eal_init().
	 	 * A common case is when drivers are loaded as shared libraries).
		 */
		i->callback();
		return;
	}

	TAILQ_FOREACH(p, &init_list, next) {
		if (i->priority >= p->priority)
			continue;
		TAILQ_INSERT_BEFORE(p, i, next);
		return;
	}

	TAILQ_INSERT_TAIL(&init_list, i, next);
}

void
rte_eal_init_call_deferred(void)
{
	struct rte_init_deferred *i;

	init_called = true;

	TAILQ_FOREACH(i, &init_list, next)
		i->callback();
}
