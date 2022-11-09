/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2022 Red Hat, Inc.
 */

#ifndef BBDEV_VDEV_H
#define BBDEV_VDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bus_vdev_driver.h>
#include <rte_bbdev_pmd.h>

static inline struct rte_bbdev *
bbdev_vdev_allocate(struct rte_vdev_device *dev, size_t private_data_size)
{
	struct rte_bbdev *bbdev;
	const char *name;

	if (dev == NULL)
		return NULL;

	name = rte_vdev_device_name(dev);
	bbdev = rte_bbdev_allocate(name);
	if (bbdev == NULL)
		return NULL;

	if (private_data_size != 0) {
		bbdev->data->dev_private = rte_zmalloc_socket(name,
			private_data_size, RTE_CACHE_LINE_SIZE,
			dev->device.numa_node);
		if (bbdev->data->dev_private == NULL) {
			rte_bbdev_release(bbdev);
			return NULL;
		}
	}

	bbdev->device = &dev->device;
	bbdev->intr_handle = NULL;
	bbdev->data->socket_id = dev->device.numa_node;

	return bbdev;
}

#ifdef __cplusplus
}
#endif

#endif /* BBDEV_VDEV_H */
