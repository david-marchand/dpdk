/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation.
 * Copyright(c) 2016 6WIND S.A.
 */

#ifndef MEMPOOL_INTERNAL_H
#define MEMPOOL_INTERNAL_H

/**
 * @file
 * Mempool internal API.
 */

#include <rte_compat.h>
#include <rte_mempool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mempool event type.
 * @internal
 */
enum rte_mempool_event {
	/** Occurs after a mempool is fully populated. */
	RTE_MEMPOOL_EVENT_READY = 0,
	/** Occurs before the destruction of a mempool begins. */
	RTE_MEMPOOL_EVENT_DESTROY = 1,
};

/**
 * @internal
 * Mempool event callback.
 *
 * rte_mempool_event_callback_register() may be called from within the callback,
 * but the callbacks registered this way will not be invoked for the same event.
 * rte_mempool_event_callback_unregister() may only be safely called
 * to remove the running callback.
 */
typedef void (rte_mempool_event_callback)(
		enum rte_mempool_event event,
		struct rte_mempool *mp,
		void *user_data);

/**
 * @internal
 * Register a callback function invoked on mempool life cycle event.
 * The function will be invoked in the process
 * that performs an action which triggers the callback.
 * Registration is process-private,
 * i.e. each process must manage callbacks on its own if needed.
 *
 * @param func
 *   Callback function.
 * @param user_data
 *   User data.
 *
 * @return
 *   0 on success, negative on failure and rte_errno is set.
 */
__rte_internal
int
rte_mempool_event_callback_register(rte_mempool_event_callback *func,
				    void *user_data);

/**
 * @internal
 * Unregister a callback added with rte_mempool_event_callback_register().
 * @p func and @p user_data must exactly match registration parameters.
 *
 * @param func
 *   Callback function.
 * @param user_data
 *   User data.
 *
 * @return
 *   0 on success, negative on failure and rte_errno is set.
 */
__rte_internal
int
rte_mempool_event_callback_unregister(rte_mempool_event_callback *func,
				      void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* MEMPOOL_INTERNAL_H */
