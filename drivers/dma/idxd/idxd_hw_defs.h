/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021 Intel Corporation
 */

#ifndef _IDXD_HW_DEFS_H_
#define _IDXD_HW_DEFS_H_

/*
 * Defines used in the data path for interacting with IDXD hardware.
 */
#define IDXD_CMD_OP_SHIFT 24
enum rte_idxd_ops {
	idxd_op_nop = 0,
	idxd_op_batch,
	idxd_op_drain,
	idxd_op_memmove,
	idxd_op_fill
};

#define IDXD_FLAG_FENCE                 (1 << 0)
#define IDXD_FLAG_COMPLETION_ADDR_VALID (1 << 2)
#define IDXD_FLAG_REQUEST_COMPLETION    (1 << 3)
#define IDXD_FLAG_CACHE_CONTROL         (1 << 8)

/**
 * Hardware descriptor used by DSA hardware, for both bursts and
 * for individual operations.
 */
struct __rte_aligned(64) idxd_hw_desc {
	uint32_t pasid;
	uint32_t op_flags;
	rte_iova_t completion;

	union {
		rte_iova_t src;      /* source address for copy ops etc. */
		rte_iova_t desc_addr; /* descriptor pointer for batch */
	};
	rte_iova_t dst;

	uint32_t size;    /* length of data for op, or batch size */

	uint16_t intr_handle; /* completion interrupt handle */

	/* remaining 26 bytes are reserved */
	uint16_t reserved[13];
};

#define IDXD_COMP_STATUS_INCOMPLETE        0
#define IDXD_COMP_STATUS_SUCCESS           1
#define IDXD_COMP_STATUS_PAGE_FAULT     0X03
#define IDXD_COMP_STATUS_INVALID_OPCODE 0x10
#define IDXD_COMP_STATUS_INVALID_SIZE   0x13
#define IDXD_COMP_STATUS_SKIPPED        0xFF /* not official IDXD error, needed as placeholder */

/**
 * Completion record structure written back by DSA
 */
struct __rte_aligned(32) idxd_completion {
	uint8_t status;
	uint8_t result;
	/* 16-bits pad here */
	uint32_t completed_size; /* data length, or descriptors for batch */

	rte_iova_t fault_address;
	uint32_t invalid_flags;
};

/*** Definitions for Intel(R) Data Streaming Accelerator  ***/

#define IDXD_CMD_SHIFT 20
enum rte_idxd_cmds {
	idxd_enable_dev = 1,
	idxd_disable_dev,
	idxd_drain_all,
	idxd_abort_all,
	idxd_reset_device,
	idxd_enable_wq,
	idxd_disable_wq,
	idxd_drain_wq,
	idxd_abort_wq,
	idxd_reset_wq,
};

/* General bar0 registers */
struct rte_idxd_bar0 {
	alignas(RTE_CACHE_LINE_SIZE) uint32_t version;    /* offset 0x00 */
	alignas(0x10) uint64_t gencap;     /* offset 0x10 */
	alignas(0x10) uint64_t wqcap;      /* offset 0x20 */
	alignas(0x10) uint64_t grpcap;     /* offset 0x30 */
	alignas(0x08) uint64_t engcap;     /* offset 0x38 */
	alignas(0x10) uint64_t opcap;      /* offset 0x40 */
	alignas(0x20) uint64_t offsets[2]; /* offset 0x60 */
	alignas(0x20) uint32_t gencfg;     /* offset 0x80 */
	alignas(0x08) uint32_t genctrl;    /* offset 0x88 */
	alignas(0x10) uint32_t gensts;     /* offset 0x90 */
	alignas(0x08) uint32_t intcause;   /* offset 0x98 */
	alignas(0x10) uint32_t cmd;        /* offset 0xA0 */
	alignas(0x08) uint32_t cmdstatus;  /* offset 0xA8 */
	alignas(0x20) uint64_t swerror[4]; /* offset 0xC0 */
};

/* workqueue config is provided by array of uint32_t. */
enum rte_idxd_wqcfg {
	wq_size_idx,       /* size is in first 32-bit value */
	wq_threshold_idx,  /* WQ threshold second 32-bits */
	wq_mode_idx,       /* WQ mode and other flags */
	wq_sizes_idx,      /* WQ transfer and batch sizes */
	wq_occ_int_idx,    /* WQ occupancy interrupt handle */
	wq_occ_limit_idx,  /* WQ occupancy limit */
	wq_state_idx,      /* WQ state and occupancy state */
};

#define WQ_MODE_SHARED    0
#define WQ_MODE_DEDICATED 1
#define WQ_PRIORITY_SHIFT 4
#define WQ_BATCH_SZ_SHIFT 5
#define WQ_STATE_SHIFT 30
#define WQ_STATE_MASK 0x3

struct rte_idxd_grpcfg {
	alignas(RTE_CACHE_LINE_SIZE) uint64_t grpwqcfg[4]; /* 64-byte register set */
	uint64_t grpengcfg;  /* offset 32 */
	uint32_t grpflags;   /* offset 40 */
};

#define GENSTS_DEV_STATE_MASK 0x03
#define CMDSTATUS_ACTIVE_SHIFT 31
#define CMDSTATUS_ACTIVE_MASK (1 << 31)
#define CMDSTATUS_ERR_MASK 0xFF

#endif
