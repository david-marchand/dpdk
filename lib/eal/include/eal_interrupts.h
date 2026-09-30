/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef EAL_INTERRUPTS_H
#define EAL_INTERRUPTS_H

#include <rte_compat.h>
#include <rte_interrupts.h>

struct rte_intr_handle {
	union {
		struct {
			int dev_fd; /**< VFIO/UIO cfg device file descriptor */
			int fd;	/**< interrupt event file descriptor */
		};
		void *windows_handle; /**< device driver handle */
	};
	uint32_t alloc_flags;	/**< flags passed at allocation */
	enum rte_intr_handle_type type;  /**< handle type */
	uint32_t max_intr;             /**< max interrupt requested */
	uint32_t nb_efd;               /**< number of available efd(event fd) */
	uint8_t efd_counter_size;      /**< size of efd counter, used for vdev */
	uint16_t nb_intr;
		/**< Max vector count, default RTE_MAX_RXTX_INTR_VEC_ID */
	int *efds;  /**< intr vectors/efds mapping */
	struct rte_epoll_event *elist; /**< intr vector epoll event */
	uint16_t vec_list_size;
	int *intr_vec;                 /**< intr vector number array */
};

/**
 * @internal
 * Return the event flags for the interrupt currently being processed.
 *
 * Must be called from an interrupt callback running on the EAL
 * interrupt thread. The returned value is a bitmask of
 * RTE_INTR_EVENT_* flags.
 *
 * @return
 *   Active event flags, or 0 if not in interrupt context or
 *   on platforms that do not support this feature.
 */
__rte_internal
uint32_t rte_intr_active_events_flags(void);

/**
 * @internal
 * @param intr_handle
 *   Pointer to the interrupt handle.
 * @param epfd
 *   Epoll instance fd which the intr vector associated to.
 * @param op
 *   The operation be performed for the vector.
 *   Operation type of {ADD, DEL}.
 * @param vec
 *   RX intr vector number added to the epoll instance wait list.
 * @param data
 *   User raw data.
 * @return
 *   - On success, zero.
 *   - On failure, a negative value.
 */
__rte_internal
int
rte_intr_rx_ctl(struct rte_intr_handle *intr_handle,
		int epfd, int op, unsigned int vec, void *data);

/**
 * @internal
 * It deletes registered eventfds.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 */
__rte_internal
void
rte_intr_free_epoll_fd(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * It enables the packet I/O interrupt event if it's necessary.
 * It creates event fd for each interrupt vector when MSIX is used,
 * otherwise it multiplexes a single event fd.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 * @param nb_efd
 *   Number of interrupt vector trying to enable.
 *   The value 0 is not allowed.
 * @return
 *   - On success, zero.
 *   - On failure, a negative value.
 */
__rte_internal
int
rte_intr_efd_enable(struct rte_intr_handle *intr_handle, uint32_t nb_efd);

/**
 * @internal
 * It disables the packet I/O interrupt event.
 * It deletes registered eventfds and closes the open fds.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 */
__rte_internal
void
rte_intr_efd_disable(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * The packet I/O interrupt on datapath is enabled or not.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 */
__rte_internal
int
rte_intr_dp_is_en(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * The interrupt handle instance allows other causes or not.
 * Other causes stand for any none packet I/O interrupts.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 */
__rte_internal
int
rte_intr_allow_others(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * The multiple interrupt vector capability of interrupt handle instance.
 * It returns zero if no multiple interrupt vector support.
 *
 * @param intr_handle
 *   Pointer to the interrupt handle.
 */
__rte_internal
int
rte_intr_cap_multiple(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Set the device fd field of interrupt handle with user
 * provided dev fd. Device fd corresponds to VFIO device fd or UIO config fd.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param fd
 *  interrupt type
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_dev_fd_set(struct rte_intr_handle *intr_handle, int fd);

/**
 * @internal
 * Returns the device fd field of the given interrupt handle instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, dev fd.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_dev_fd_get(const struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Set the max intr field of interrupt handle with user
 * provided max intr value.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param max_intr
 *  interrupt type
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_max_intr_set(struct rte_intr_handle *intr_handle, int max_intr);

/**
 * @internal
 * Returns the max intr field of the given interrupt handle instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, max intr.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_max_intr_get(const struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Set the number of event fd field of interrupt handle
 * with user provided available event file descriptor value.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param nb_efd
 *  Available event fd
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_nb_efd_set(struct rte_intr_handle *intr_handle, int nb_efd);

/**
 * @internal
 * Returns the number of available event fd field of the given interrupt handle
 * instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, nb_efd
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_nb_efd_get(const struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Returns the number of interrupt vector field of the given interrupt handle
 * instance. This field is to configured on device probe time, and based on
 * this value efds and elist arrays are dynamically allocated. By default
 * this value is set to RTE_MAX_RXTX_INTR_VEC_ID.
 * For eg. in case of PCI device, its msix size is queried and efds/elist
 * arrays are allocated accordingly.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, nb_intr
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_nb_intr_get(const struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Set the event fd counter size field of interrupt handle
 * with user provided efd counter size.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param efd_counter_size
 *  size of efd counter.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_efd_counter_size_set(struct rte_intr_handle *intr_handle,
			      uint8_t efd_counter_size);

/**
 * @internal
 * Set the event fd array index with the given fd.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param index
 *  efds array index to be set
 * @param fd
 *  event fd
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_efds_index_set(struct rte_intr_handle *intr_handle, int index, int fd);

/**
 * @internal
 * Returns the fd value of event fds array at a given index.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param index
 *  efds array index to be returned
 *
 * @return
 *  - On success, fd
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_efds_index_get(const struct rte_intr_handle *intr_handle, int index);

/**
 * @internal
 * Allocates the memory of interrupt vector list array, with size defining the
 * number of elements required in the array.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param name
 *  Name assigned to the allocation, or NULL.
 * @param size
 *  Number of element required in the array.
 *
 * @return
 *  - On success, zero
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_vec_list_alloc(struct rte_intr_handle *intr_handle, const char *name,
			int size);

/**
 * @internal
 * Sets the vector value at given index of interrupt vector list field of given
 * interrupt handle.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param index
 *  intr_vec array index to be set
 * @param vec
 *  Interrupt vector value.
 *
 * @return
 *  - On success, zero
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_vec_list_index_set(struct rte_intr_handle *intr_handle, int index,
			    int vec);

/**
 * @internal
 * Returns the vector value at the given index of interrupt vector list array.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param index
 *  intr_vec array index to be returned
 *
 * @return
 *  - On success, interrupt vector
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_vec_list_index_get(const struct rte_intr_handle *intr_handle,
			    int index);

/**
 * @internal
 * Frees the memory allocated for interrupt vector list array.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, zero
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
void
rte_intr_vec_list_free(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Reallocates the size efds and elist array based on size provided by user.
 * By default efds and elist array are allocated with default size
 * RTE_MAX_RXTX_INTR_VEC_ID on interrupt handle array creation. Later on device
 * probe, device may have capability of more interrupts than
 * RTE_MAX_RXTX_INTR_VEC_ID. Using this API, PMDs can reallocate the arrays as
 * per the max interrupts capability of device.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param size
 *  efds and elist array size.
 *
 * @return
 *  - On success, zero
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_event_list_update(struct rte_intr_handle *intr_handle, int size);

/**
 * @internal
 * Returns the Windows handle of the given interrupt instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 *
 * @return
 *  - On success, Windows handle.
 *  - On failure, NULL.
 */
__rte_internal
void *
rte_intr_instance_windows_handle_get(struct rte_intr_handle *intr_handle);

/**
 * @internal
 * Set the Windows handle for the given interrupt instance.
 *
 * @param intr_handle
 *  pointer to the interrupt handle.
 * @param windows_handle
 *  Windows handle to be set.
 *
 * @return
 *  - On success, zero
 *  - On failure, a negative value and rte_errno is set.
 */
__rte_internal
int
rte_intr_instance_windows_handle_set(struct rte_intr_handle *intr_handle,
				     void *windows_handle);

#endif /* EAL_INTERRUPTS_H */
