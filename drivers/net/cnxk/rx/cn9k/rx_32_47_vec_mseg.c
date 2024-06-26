/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2022 Marvell.
 */

#include "cn9k_ethdev.h"
#include "cn9k_rx.h"

#if !defined(CNXK_DIS_TMPLT_FUNC)

#define R(name, flags)                                                         \
	NIX_RX_RECV_VEC_MSEG(cn9k_nix_recv_pkts_vec_mseg_##name, flags)

NIX_RX_FASTPATH_MODES_32_47
#undef R

#endif
