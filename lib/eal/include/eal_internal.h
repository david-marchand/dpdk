/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2018 Intel Corporation
 */

#ifndef EAL_INTERNAL_H
#define EAL_INTERNAL_H

/**
 * @file
 * EAL internal API.
 */

#include <stdbool.h>
#include <stdint.h>

#include <rte_compat.h>
#include <rte_spinlock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the OS-specific EAL base address.
 *
 * @return
 *    The base address.
 */
__rte_internal
uint64_t rte_eal_get_baseaddr(void);

/**
 * @internal
 * Iterate to the next driver path.
 *
 * This function iterates through the list of dynamically loaded drivers,
 * or driver paths that were specified via -d or --driver-path command-line
 * options during EAL initialization.
 *
 * @param start
 *   Starting iteration point. The iteration will start at the first driver path if NULL.
 * @param cmdline_only
 *   If true, only iterate paths from command line (-d flags).
 *   If false, iterate all paths including those expanded from directories.
 *
 * @return
 *   Next driver path string, NULL if there is none.
 */
__rte_internal
const char *
rte_eal_driver_path_next(const char *start, bool cmdline_only);

/**
 * @internal
 * Iterate over all driver paths.
 *
 * This macro provides a convenient way to iterate through all driver paths
 * that were loaded via -d flags during EAL initialization.
 *
 * @param path
 *   Iterator variable of type const char *
 * @param cmdline_only
 *   If true, only iterate paths from command line (-d flags).
 *   If false, iterate all paths including those expanded from directories.
 */
#define RTE_EAL_DRIVER_PATH_FOREACH(path, cmdline_only) \
	for (path = rte_eal_driver_path_next(NULL, cmdline_only); \
	     path != NULL; \
	     path = rte_eal_driver_path_next(path, cmdline_only))

/**
 * @internal
 * Get count of driver paths.
 *
 * @param cmdline_only
 *   If true, only count paths from command line (-d flags).
 *   If false, count all paths including those expanded from directories.
 *
 * @return
 *   Number of driver paths.
 */
__rte_internal
unsigned int
rte_eal_driver_path_count(bool cmdline_only);

__rte_internal
rte_spinlock_t *
rte_mcfg_ethdev_get_lock(void);

#ifdef __cplusplus
}
#endif

#endif /* EAL_INTERNAL_H */
