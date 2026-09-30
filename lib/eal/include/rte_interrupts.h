/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _RTE_INTERRUPTS_H_
#define _RTE_INTERRUPTS_H_

#include <stdbool.h>

#include <rte_bitops.h>
#include <rte_common.h>
#include <rte_epoll.h>

/**
 * @file
 *
 * The RTE interrupt interface provides functions to register/unregister
 * callbacks for a specific interrupt.
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Interrupt handle */
struct rte_intr_handle;

/** Interrupt instance allocation flags
 * @see rte_intr_instance_alloc
 */

/** Interrupt instance will not be shared between primary and secondary processes. */
#define RTE_INTR_INSTANCE_F_PRIVATE     UINT32_C(0)
/** Interrupt instance will be shared between primary and secondary processes. */
#define RTE_INTR_INSTANCE_F_SHARED      RTE_BIT32(0)

#define RTE_MAX_RXTX_INTR_VEC_ID      512
#define RTE_INTR_VEC_ZERO_OFFSET      0
#define RTE_INTR_VEC_RXTX_OFFSET      1

/** Interrupt event flags returned by rte_intr_active_events_flags() */
#define RTE_INTR_EVENT_IN    RTE_BIT32(0)  /**< Data available to read */
#define RTE_INTR_EVENT_ERR   RTE_BIT32(1)  /**< Error condition on fd */
#define RTE_INTR_EVENT_HUP   RTE_BIT32(2)  /**< Hang up / disconnect */
#define RTE_INTR_EVENT_RDHUP RTE_BIT32(3)  /**< Read hang up / disconnect */

/**
 * The interrupt source type, e.g. UIO, VFIO, ALARM etc.
 */
enum rte_intr_handle_type {
	RTE_INTR_HANDLE_UNKNOWN = 0,  /**< generic unknown handle */
	RTE_INTR_HANDLE_UIO,          /**< uio device handle */
	RTE_INTR_HANDLE_UIO_INTX,     /**< uio generic handle */
	RTE_INTR_HANDLE_VFIO_LEGACY,  /**< vfio device handle (legacy) */
	RTE_INTR_HANDLE_VFIO_MSI,     /**< vfio device handle (MSI) */
	RTE_INTR_HANDLE_VFIO_MSIX,    /**< vfio device handle (MSIX) */
	RTE_INTR_HANDLE_ALARM,        /**< alarm handle */
	RTE_INTR_HANDLE_EXT,          /**< external handler */
	RTE_INTR_HANDLE_VDEV,         /**< virtual device */
	RTE_INTR_HANDLE_DEV_EVENT,    /**< device event handle */
	RTE_INTR_HANDLE_VFIO_REQ,     /**< VFIO request handle */
	RTE_INTR_HANDLE_MAX           /**< count of elements */
};

/** Function to be registered for the specific interrupt */
typedef void (*rte_intr_callback_fn)(void *cb_arg);

/**
 * Function to call after a callback is unregistered.
 * Can be used to close fd and free cb_arg.
 */
typedef void (*rte_intr_unregister_callback_fn)(struct rte_intr_handle *intr_handle,
						void *cb_arg);

/**
 * It registers the callback for the specific interrupt. Multiple
 * callbacks can be registered at the same time.
 * @param intr_handle
 *  Pointer to the interrupt handle.
 * @param cb
 *  callback address.
 * @param cb_arg
 *  address of parameter for callback.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
int rte_intr_callback_register(const struct rte_intr_handle *intr_handle,
				rte_intr_callback_fn cb, void *cb_arg);

/**
 * It unregisters the callback according to the specified interrupt handle.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param cb
 *  callback address.
 * @param cb_arg
 *  address of parameter for callback, (void *)-1 means to remove all
 *  registered which has the same callback address.
 *
 * @return
 *  - On success, return the number of callback entities removed.
 *  - On failure, a negative value.
 */
int rte_intr_callback_unregister(const struct rte_intr_handle *intr_handle,
				rte_intr_callback_fn cb, void *cb_arg);

/**
 * Unregister the callback according to the specified interrupt handle,
 * after it's no longer active. Fail if source is not active.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param cb_fn
 *  callback address.
 * @param cb_arg
 *  address of parameter for callback, (void *)-1 means to remove all
 *  registered which has the same callback address.
 * @param ucb_fn
 *  callback to call before cb is unregistered (optional).
 *  can be used to close fd and free cb_arg.
 *
 * @return
 *  - On success, return the number of callback entities marked for remove.
 *  - On failure, a negative value.
 */
int
rte_intr_callback_unregister_pending(const struct rte_intr_handle *intr_handle,
				rte_intr_callback_fn cb_fn, void *cb_arg,
				rte_intr_unregister_callback_fn ucb_fn);

/**
 * Loop until rte_intr_callback_unregister() succeeds.
 * After a call to this function,
 * the callback provided by the specified interrupt handle is unregistered.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param cb
 *  callback address.
 * @param cb_arg
 *  address of parameter for callback, (void *)-1 means to remove all
 *  registered which has the same callback address.
 *
 * @return
 *  - On success, return the number of callback entities removed.
 *  - On failure, a negative value.
 */
int
rte_intr_callback_unregister_sync(const struct rte_intr_handle *intr_handle,
				rte_intr_callback_fn cb, void *cb_arg);

/**
 * It enables the interrupt for the specified handle.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
int rte_intr_enable(const struct rte_intr_handle *intr_handle);

/**
 * It disables the interrupt for the specified handle.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
int rte_intr_disable(const struct rte_intr_handle *intr_handle);

/**
 * It acknowledges an interrupt raised for the specified handle.
 *
 * This function should be called at the end of each interrupt handler either
 * from application or driver, so that currently raised interrupt is acked and
 * further new interrupts are raised.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
int rte_intr_ack(const struct rte_intr_handle *intr_handle);

/**
 * Check if currently executing in interrupt context
 *
 * @return
 *  - non zero in case of interrupt context
 *  - zero in case of process context
 */
int rte_thread_is_intr(void);

/**
 * It allocates memory for interrupt instance. API takes flag as an argument
 * which define from where memory should be allocated i.e. using DPDK memory
 * management library APIs or normal heap allocation.
 * Default memory allocation for event fds and event list array is done which
 * can be realloced later based on size of MSIX interrupts supported by a PCI
 * device.
 *
 * This function should be called from application or driver, before calling
 * any of the interrupt APIs.
 *
 * @param flags
 *  See RTE_INTR_INSTANCE_F_* flags definitions.
 *
 * @return
 *  - On success, address of interrupt handle.
 *  - On failure, NULL.
 */
struct rte_intr_handle *
rte_intr_instance_alloc(uint32_t flags);

/**
 * Free the memory allocated for interrupt handle resources.
 *
 * @param intr_handle
 *  Interrupt handle allocated with rte_intr_instance_alloc().
 *  If intr_handle is NULL, no operation is performed.
 */
void
rte_intr_instance_free(struct rte_intr_handle *intr_handle);

/**
 * Set the fd field of interrupt handle with user provided
 * file descriptor.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param fd
 *  file descriptor value provided by user.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
int
rte_intr_fd_set(struct rte_intr_handle *intr_handle, int fd);

/**
 * Returns the fd field of the given interrupt handle instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, fd field.
 *  - On failure, a negative value.
 */
int
rte_intr_fd_get(const struct rte_intr_handle *intr_handle);

/**
 * Set the type field of interrupt handle with user provided
 * interrupt type.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param type
 *  interrupt type
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
int
rte_intr_type_set(struct rte_intr_handle *intr_handle,
		  enum rte_intr_handle_type type);

/**
 * Returns the type field of the given interrupt handle instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, interrupt type
 *  - On failure, RTE_INTR_HANDLE_UNKNOWN.
 */
enum rte_intr_handle_type
rte_intr_type_get(const struct rte_intr_handle *intr_handle);

#ifdef __cplusplus
}
#endif

#endif
