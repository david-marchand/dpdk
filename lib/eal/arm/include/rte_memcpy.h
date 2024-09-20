/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 RehiveTech. All rights reserved.
 */

#ifndef _RTE_MEMCPY_ARM_H_
#define _RTE_MEMCPY_ARM_H_

#if defined(RTE_USE_CC_MEMCPY) || !defined(RTE_ARCH_ARM64_MEMCPY)

#define RTE_CC_MEMCPY
#include <generic/rte_memcpy.h>

#else

#ifdef RTE_ARCH_64
#include <rte_memcpy_64.h>
#else
#include <rte_memcpy_32.h>
#endif

#endif /* RTE_USE_CC_MEMCPY */

#endif /* _RTE_MEMCPY_ARM_H_ */
