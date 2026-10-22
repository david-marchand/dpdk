/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Intel Corporation
 */

#ifndef CRYPTODEV_PRIVATE_H
#define CRYPTODEV_PRIVATE_H

#include <rte_cryptodev.h>

/* Reset crypto device fastpath APIs to dummy values. */
void
cryptodev_fp_ops_reset(struct rte_crypto_fp_ops *fp_ops);

/* Setup crypto device fastpath APIs. */
void
cryptodev_fp_ops_set(struct rte_crypto_fp_ops *fp_ops, const struct rte_cryptodev *dev);

#endif /* CRYPTODEV_PRIVATE_H */
