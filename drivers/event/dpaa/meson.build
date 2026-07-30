# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2018 NXP

if not is_linux
    build = false
    reason = 'only supported on Linux'
endif
deps += ['net_dpaa', 'crypto_dpaa_sec']
sources = files('dpaa_eventdev.c')

cflags += no_packed_not_aligned_cflag

includes += include_directories('../../crypto/dpaa_sec/')
