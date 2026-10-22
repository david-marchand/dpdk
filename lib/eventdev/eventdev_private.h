/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef EVENTDEV_PRIVATE_H
#define EVENTDEV_PRIVATE_H

struct rte_event_fp_ops;
struct rte_eventdev;

/* Reset eventdevice fastpath APIs to dummy values. */
void
event_dev_fp_ops_reset(struct rte_event_fp_ops *fp_op);

/* Set eventdevice fastpath APIs to event device values. */
void
event_dev_fp_ops_set(struct rte_event_fp_ops *fp_ops, const struct rte_eventdev *dev);

#endif /* EVENTDEV_PRIVATE_H */
