/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

/**
 * @file
 *
 * API for error cause tracking
 */

#ifndef _RTE_ERRNO_H_
#define _RTE_ERRNO_H_

#include <rte_per_lcore.h>

#ifdef __cplusplus
extern "C" {
#endif

RTE_DECLARE_PER_LCORE(int, _rte_errno); /**< Per core error number. */

/**
 * Error number value, stored per-thread, which can be queried after
 * calls to certain functions to determine why those functions failed.
 *
 * Uses standard values from errno.h wherever possible, with a small number
 * of additional possible values for RTE-specific conditions.
 */
#define rte_errno RTE_PER_LCORE(_rte_errno)

/**
 * Function which returns a printable string describing a particular
 * error code. For non-RTE-specific error codes, this function returns
 * the value from the libc strerror function.
 *
 * @param errnum
 *   The error number to be looked up - generally the value of rte_errno
 * @return
 *   A pointer to a thread-local string containing the text describing
 *   the error.
 */
const char *rte_strerror(int errnum);

/** Error types */
enum {
	RTE_MIN_ERRNO = 1000, /**< Start numbering above std errno vals */

	E_RTE_SECONDARY, /**< Operation not allowed in secondary processes */
	E_RTE_NO_CONFIG, /**< Missing rte_config */

	RTE_MAX_ERRNO    /**< Max RTE error number */
};

#ifdef __cplusplus
}
#endif

#endif /* _RTE_ERRNO_H_ */
