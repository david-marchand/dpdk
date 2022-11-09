/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2022 Red Hat, Inc.
 */

#ifndef BBDEV_PCI_H
#define BBDEV_PCI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bus_pci_driver.h>
#include <rte_bbdev_pmd.h>

static inline struct rte_bbdev *
bbdev_pci_allocate(struct rte_pci_device *pci_dev, size_t private_data_size)
{
	struct rte_bbdev *bbdev;
	const char *name;

	if (pci_dev == NULL)
		return NULL;

	name = pci_dev->device.name;
	bbdev = rte_bbdev_allocate(name);
	if (bbdev == NULL)
		return NULL;

	if (private_data_size != 0) {
		bbdev->data->dev_private = rte_zmalloc_socket(name,
			private_data_size, RTE_CACHE_LINE_SIZE,
			pci_dev->device.numa_node);
		if (bbdev->data->dev_private == NULL) {
			rte_bbdev_release(bbdev);
			return NULL;
		}
	}

	bbdev->device = &pci_dev->device;
	bbdev->intr_handle = pci_dev->intr_handle;
	bbdev->data->socket_id = pci_dev->device.numa_node;
	return bbdev;
}

typedef int (*bbdev_pci_callback_t)(struct rte_bbdev *bbdev);

static inline int
bbdev_pci_generic_probe(struct rte_pci_device *pci_dev,
	size_t private_data_size, bbdev_pci_callback_t dev_init)
{
	struct rte_bbdev *bbdev;
	int ret;

	bbdev = bbdev_pci_allocate(pci_dev, private_data_size);
	if (bbdev == NULL)
		return -ENOMEM;

	if (*dev_init == NULL)
		return -EINVAL;
	ret = dev_init(bbdev);
	if (ret != 0)
		rte_bbdev_release(bbdev);

	return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* BBDEV_PCI_H */
