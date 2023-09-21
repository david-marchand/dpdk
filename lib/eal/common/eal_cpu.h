/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2023 Red Hat, Inc.
 */

#ifndef EAL_CPU_H
#define EAL_CPU_H

#include <stdbool.h>
#include <stdint.h>

#include <rte_compat.h>

__rte_internal
bool rte_cpu_is_x86(void);

__rte_internal
bool rte_cpu_x86_is_amd(void);
__rte_internal
bool rte_cpu_x86_is_intel(void);

__rte_internal
uint8_t rte_cpu_x86_brand(void);
__rte_internal
uint8_t rte_cpu_x86_family(void);
__rte_internal
uint8_t rte_cpu_x86_model(void);

#endif /* EAL_CPU_H */
