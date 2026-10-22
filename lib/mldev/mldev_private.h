/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2022 Marvell.
 */

#ifndef MLDEV_PRIVATE_H
#define MLDEV_PRIVATE_H

#include <stdint.h>

struct rte_ml_dev;

/* Allocate a new mldev slot for an ML device. */
struct rte_ml_dev *
rte_ml_dev_pmd_allocate(const char *name, uint8_t socket_id);

/* Release the specified mldev device. */
int
rte_ml_dev_pmd_release(struct rte_ml_dev *dev);

#endif /* MLDEV_PRIVATE_H */
