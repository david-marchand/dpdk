/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2019 Vladimir Medvedkin <medvedkinv@gmail.com>
 * Copyright(c) 2021 Intel Corporation
 */

#ifndef THASH_H
#define THASH_H

#include <stdint.h>

/**
 *  @brief Generates a random polynomial
 *
 * @param poly_degree
 *   degree of the polynomial
 *
 * @return
 *   random polynomial
 */
uint32_t
thash_get_rand_poly(uint32_t poly_degree);

#endif /* THASH_H */
