/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2023-2024 Broadcom
 * All rights reserved.
 */

#ifndef _CFA_BLD_P70_H_
#define _CFA_BLD_P70_H_

#include "sys_util.h"
#include "cfa_bld_defs.h"
#include "cfa_bld_p70_field_ids.h"

/**
 * Maximum key array size
 */
#define CFA_P70_KEY_MAX_FIELD_CNT                                              \
	MAX((uint16_t)CFA_P70_EM_KEY_LAYOUT_MAX_FLD,                           \
	    (uint16_t)CFA_P70_WC_TCAM_FKB_MAX_FLD)
#define CFA_P70_ACT_MAX_TEMPLATE_SZ sizeof(struct cfa_bld_p70_action_template)

#define CFA_P70_PROF_MAX_KEYS 4
enum cfa_p70_mac_sel_mode {
	CFA_P70_MAC_SEL_MODE_FIRST = 0,
	CFA_P70_MAC_SEL_MODE_LOWEST = 1
};

struct cfa_p70_prof_key_cfg {
	uint8_t mac_sel[CFA_P70_PROF_MAX_KEYS];
#define CFA_PROF_P70_MAC_SEL_DMAC0 (1 << 0)
#define CFA_PROF_P70_MAC_SEL_T_MAC0 (1 << 1)
#define CFA_PROF_P70_MAC_SEL_OUTERMOST_MAC0 (1 << 2)
#define CFA_PROF_P70_MAC_SEL_DMAC1 (1 << 3)
#define CFA_PROF_P70_MAC_SEL_T_MAC1 (1 << 4)
#define CFA_PROF_P70_MAC_OUTERMOST_MAC1 (1 << 5)
	uint8_t vlan_sel[CFA_P70_PROF_MAX_KEYS];
#define CFA_PROF_P70_VLAN_SEL_INNER_HDR 0
#define CFA_PROF_P70_VLAN_SEL_TUNNEL_HDR 1
#define CFA_PROF_P70_VLAN_SEL_OUTERMOST_HDR 2
	uint8_t pass_cnt;
	enum cfa_p70_mac_sel_mode mode;
};

/*
 * Field id remap function pointer. Passed by cfa-v3 caller
 * to builder apis if the caller requires the apis to remap
 * the field ids before using them to update key/action layout
 * objects. An example of one such api is action_compute_ptr()
 * which updates the offsets for the modify/encap/source/stat
 * records in the action record. If the caller is remapping
 * the field ids (to save memory in fw builds for example), then
 * this remap api is required to be passed. If passed as NULL,
 * the field ids are not remapped and used directly to index
 * into the layout.
 *
 * @param Input field id
 *
 * @return Remapped field id on success, UNIT16_MAX on failure.
 */
typedef uint16_t(cfa_fld_remap)(uint16_t);

/**
 * CFA P70 action layout definition
 */

enum action_type_p70 {
	/** Select this type to build an Full Action Record Object
	 */
	CFA_P70_ACT_OBJ_TYPE_FULL_ACT,
	/** Select this type to build an Compact Action Record Object
	 */
	CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT,
	/** Select this type to build an MCG Action Record Object
	 */
	CFA_P70_ACT_OBJ_TYPE_MCG_ACT,
	/** Select this type to build Standalone Modify Action Record Object */
	CFA_P70_ACT_OBJ_TYPE_MODIFY,
	/** Select this type to build Standalone Stat Action Record Object */
	CFA_P70_ACT_OBJ_TYPE_STAT,
	/** Select this type to build Standalone Source Action Record Object */
	CFA_P70_ACT_OBJ_TYPE_SRC_PROP,
	/** Select this type to build Standalone Encap Action Record Object */
	CFA_P70_ACT_OBJ_TYPE_ENCAP,
};

enum stat_op_p70 {
	/** Set to statistic to ingress to CFA
	 */
	CFA_P70_STAT_OP_INGRESS = 0,
	/** Set to statistic to egress from CFA
	 */
	CFA_P70_STAT_OP_EGRESS = 1,
};

enum stat_type_p70 {
	/** Set to statistic to Forward packet count(64b)/Forward byte
	 *  count(64b)
	 */
	CFA_P70_STAT_COUNTER_SIZE_16B = 0,
	/** Set to statistic to Forward packet count(64b)/Forward byte
	 *  count(64b)/ TCP Flags(16b)/Timestamp(32b)
	 */
	CFA_P70_STAT_COUNTER_SIZE_24B = 1,
	/** Set to statistic to Forward packet count(64b)/Forward byte
	 *  count(64b)/Meter(drop or red) packet count(64b)/Meter(drop
	 *  or red) byte count(64b)
	 */
	CFA_P70_STAT_COUNTER_SIZE_32B = 2,
	/** Set to statistic to Forward packet count(64b)/Forward byte
	 *  count(64b)/Meter(drop or red) packet count(38b)/Meter(drop
	 *  or red) byte count(42b)/TCP Flags(16b)/Timestamp(32b)
	 */
	CFA_P70_STAT_COUNTER_SIZE_32B_ALL = 3,
};

enum encap_vtag_p70 {
	CFA_P70_ACT_ENCAP_VTAGS_PUSH_0 = 0,
	CFA_P70_ACT_ENCAP_VTAGS_PUSH_1,
	CFA_P70_ACT_ENCAP_VTAGS_PUSH_2
};

enum encap_l3_p70 {
	/** Set to disable any L3 encapsulation
	 * processing, default
	 */
	CFA_P70_ACT_ENCAP_L3_NONE = 0,
	/** Set to enable L3 IPv4 encapsulation
	 */
	CFA_P70_ACT_ENCAP_L3_IPV4 = 4,
	/** Set to enable L3 IPv6 encapsulation
	 */
	CFA_P70_ACT_ENCAP_L3_IPV6 = 5,
	/** Set to enable L3 MPLS 8847 encapsulation
	 */
	CFA_P70_ACT_ENCAP_L3_MPLS_8847 = 6,
	/** Set to enable L3 MPLS 8848 encapsulation
	 */
	CFA_P70_ACT_ENCAP_L3_MPLS_8848 = 7
};

enum encap_tunnel_p70 {
	/** Set to disable Tunnel header encapsulation
	 * processing, default
	 */
	CFA_P70_ACT_ENCAP_TNL_NONE = 0,
	/** Set to enable Tunnel Generic Full header
	 * encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_GENERIC_FULL,
	/** Set to enable VXLAN header encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_VXLAN,
	/** Set to enable NGE (VXLAN2) header encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_NGE,
	/** Set to enable NVGRE header encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_NVGRE,
	/** Set to enable GRE header encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_GRE,
	/** Set to enable Generic header after Tunnel
	 * L4 encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TL4,
	/** Set to enable Generic header after Tunnel
	 * encapsulation
	 */
	CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TNL
};

enum source_rec_type_p70 {
	/** Set to Source MAC Address
	 */
	CFA_P70_SOURCE_MAC = 0,
	/** Set to Source MAC and IPv4 Addresses
	 */
	CFA_P70_SOURCE_MAC_IPV4 = 1,
	/** Set to Source MAC and IPv6 Addresses
	 */
	CFA_P70_SOURCE_MAC_IPV6 = 2,
};

/**
 * From CFA phase 7.0 onwards, setting the modify vector bit
 * 'ACT_MODIFY_TUNNEL_MODIFY' requires corresponding data fields to be
 * set. This enum defines the parameters that determine the
 * layout of this associated data fields. This structure
 * is not used for versions older than CFA Phase 7.0 and setting
 * the 'ACT_MODIFY_TUNNEL_MODIFY' bit will just delete the internal tunnel
 */
enum tunnel_modify_mode_p70 {
	/* No change to tunnel protocol */
	CFA_P70_ACT_MOD_TNL_NO_PROTO_CHANGE = 0,
	/* 8-bit tunnel protocol change */
	CFA_P70_ACT_MOD_TNL_8B_PROTO_CHANGE = 1,
	/* 16-bit tunnel protocol change */
	CFA_P70_ACT_MOD_TNL_16B_PROTO_CHANGE = 2,
	CFA_P70_ACT_MOD_TNL_MAX
};

/**
 * Action object template structure
 *
 * Template structure presents data fields that are necessary to know
 * at the beginning of Action Builder (AB) processing. Like before the
 * AB compilation. One such example could be a template that is
 * flexible in size (Encap Record) and the presence of these fields
 * allows for determining the template size as well as where the
 * fields are located in the record.
 *
 * The template may also present fields that are not made visible to
 * the caller by way of the action fields.
 *
 * Template fields also allow for additional checking on user visible
 * fields. One such example could be the encap pointer behavior on a
 * CFA_P70_ACT_OBJ_TYPE_ACT or CFA_P70_ACT_OBJ_TYPE_ACT_SRAM.
 */
struct cfa_bld_p70_action_template {
	/** Action Object type
	 *
	 * Controls the type of the Action Template
	 */
	enum action_type_p70 obj_type;

	/** Action Control
	 *
	 * Controls the internals of the Action Template
	 *
	 * act is valid when:
	 * ((obj_type == CFA_P70_ACT_OBJ_TYPE_FULL_ACT)
	 *   ||
	 *  (obj_type == CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT))
	 *
	 * Specifies whether each action is to be in-line or not.
	 */
	struct {
		/** Set to true to enable statistics
		 */
		uint8_t stat_enable;
		/** Set to true to enable statistics to be inlined
		 */
		uint8_t stat_inline;
		/** Set to true to enable statistics 1
		 */
		uint8_t stat1_enable;
		/** Set to true to enable statistics 1 to be inlined
		 */
		uint8_t stat1_inline;
		/** Set to true to enable encapsulation
		 */
		uint8_t encap_enable;
		/** Set to true to enable encapsulation to be inlined
		 */
		uint8_t encap_inline;
		/** Set to true to align the encap record to cache
		 *  line
		 */
		uint8_t encap_align;
		/** Set to true to source
		 */
		uint8_t source_enable;
		/** Set to true to enable source to be inlined
		 */
		uint8_t source_inline;
		/** Set to true to enable modification
		 */
		uint8_t mod_enable;
		/** Set to true to enable modify to be inlined
		 */
		uint8_t mod_inline;
		/** Set to true to enable subsequent MCGs
		 */
		uint8_t mcg_subseq_enable;
	} act;

	/** Statistic Control
	 * Controls the type of statistic the template is describing
	 *
	 * stat is valid when:
	 * ((obj_type == CFA_P70_ACT_OBJ_TYPE_FULL_ACT) ||
	 *  (obj_type == CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT)) &&
	 *   act.stat_enable || act.stat_inline)
	 */
	struct {
		enum stat_op_p70 op;
		enum stat_type_p70 type;
	} stat;

	/** Encap Control
	 * Controls the type of encapsulation the template is
	 * describing
	 *
	 * encap is valid when:
	 * ((obj_type == CFA_P70_ACT_OBJ_TYPE_FULL_ACT) ||
	 *  (obj_type == CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT) &&
	 *   act.encap_enable || act.encap_inline)
	 */
	struct {
		/** Set to true to enable L2 capability in the
		 *  template
		 */
		uint8_t l2_enable;
		/** vtag controls the Encap Vector - VTAG Encoding, 4 bits
		 *
		 * <ul>
		 * <li> CFA_P70_ACT_ENCAP_VTAGS_PUSH_0, default, no VLAN
		 *      Tags applied
		 * <li> CFA_P70_ACT_ENCAP_VTAGS_PUSH_1, adds capability to
		 *      set 1 VLAN Tag. Action Template compile adds
		 *      the following field to the action object
		 *      TF_ER_VLAN1
		 * <li> CFA_P70_ACT_ENCAP_VTAGS_PUSH_2, adds capability to
		 *      set 2 VLAN Tags. Action Template compile adds
		 *      the following fields to the action object
		 *      TF_ER_VLAN1 and TF_ER_VLAN2
		 * </ul>
		 */
		enum encap_vtag_p70 vtag;

		/*
		 * The remaining fields are NOT supported when
		 * direction is RX and ((obj_type ==
		 * CFA_P70_ACT_OBJ_TYPE_ACT) && act.encap_enable).
		 * cfa_bld_p70_action_compile_layout will perform the
		 * checking and skip remaining fields.
		 */
		/** L3 Encap controls the Encap Vector - L3 Encoding,
		 *  3 bits. Defines the type of L3 Encapsulation the
		 *  template is describing.
		 * <ul>
		 * <li> CFA_P70_ACT_ENCAP_L3_NONE, default, no L3
		 *      Encapsulation processing.
		 * <li> CFA_P70_ACT_ENCAP_L3_IPV4, enables L3 IPv4
		 *      Encapsulation.
		 * <li> CFA_P70_ACT_ENCAP_L3_IPV6, enables L3 IPv6
		 *      Encapsulation.
		 * <li> CFA_P70_ACT_ENCAP_L3_MPLS_8847, enables L3 MPLS
		 *      8847 Encapsulation.
		 * <li> CFA_P70_ACT_ENCAP_L3_MPLS_8848, enables L3 MPLS
		 *      8848 Encapsulation.
		 * </ul>
		 */
		enum encap_l3_p70 l3;

#define CFA_P70_ACT_ENCAP_MAX_MPLS_LABELS 8
		/** 1-8 labels, valid when
		 * (l3 == CFA_P70_ACT_ENCAP_L3_MPLS_8847) ||
		 * (l3 == CFA_P70_ACT_ENCAP_L3_MPLS_8848)
		 *
		 * MAX number of MPLS Labels 8.
		 */
		uint8_t l3_num_mpls_labels;

		/** Set to true to enable L4 capability in the
		 * template.
		 *
		 * true adds TF_EN_UDP_SRC_PORT and
		 * TF_EN_UDP_DST_PORT to the template.
		 */
		uint8_t l4_enable;

		/** Tunnel Encap controls the Encap Vector - Tunnel
		 *  Encap, 3 bits. Defines the type of Tunnel
		 *  encapsulation the template is describing
		 * <ul>
		 * <li> CFA_P70_ACT_ENCAP_TNL_NONE, default, no Tunnel
		 *      Encapsulation processing.
		 * <li> CFA_P70_ACT_ENCAP_TNL_GENERIC_FULL
		 * <li> CFA_P70_ACT_ENCAP_TNL_VXLAN. NOTE: Expects
		 *      l4_enable set to true;
		 * <li> CFA_P70_ACT_ENCAP_TNL_NGE. NOTE: Expects l4_enable
		 *      set to true;
		 * <li> CFA_P70_ACT_ENCAP_TNL_NVGRE. NOTE: only valid if
		 *      l4_enable set to false.
		 * <li> CFA_P70_ACT_ENCAP_TNL_GRE.NOTE: only valid if
		 *      l4_enable set to false.
		 * <li> CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TL4
		 * <li> CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TNL
		 * </ul>
		 */
		enum encap_tunnel_p70 tnl;

#define CFA_P70_ACT_ENCAP_MAX_TUNNEL_GENERIC_SIZE 128
		/** Number of bytes of generic tunnel header,
		 * valid when
		 * (tnl == CFA_P70_ACT_ENCAP_TNL_GENERIC_FULL) ||
		 * (tnl == CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TL4) ||
		 * (tnl == CFA_P70_ACT_ENCAP_TNL_GENERIC_AFTER_TNL)
		 */
		uint8_t tnl_generic_size;

#define CFA_P70_ACT_ENCAP_MAX_OPLEN 15
		/** Number of 32b words of nge options,
		 * valid when
		 * (tnl == CFA_P70_ACT_ENCAP_TNL_NGE)
		 */
		uint8_t tnl_nge_op_len;

		/** Set to true to enable MAC/VLAN/IP/TNL overrides in the
		 *  template
		 */
		bool encap_override;
		/* Currently not planned */
		/* Custom Header */
		/*	uint8_t custom_enable; */
	} encap;

	/** Modify Control
	 *
	 * Controls the type of the Modify Action the template is
	 * describing
	 *
	 * modify is valid when:
	 * ((obj_type == CFA_P70_ACT_OBJ_TYPE_FULL_ACT) ||
	 *  (obj_type == CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT) &&
	 *   act.modify_enable || act.modify_inline)
	 */
/** Set to enable Modify of Metadata
 */
#define CFA_P70_ACT_MODIFY_META 0x1
/** Set to enable Delete of Outer VLAN
 */
#define CFA_P70_ACT_MODIFY_DEL_OVLAN 0x2
/** Set to enable Delete of Inner VLAN
 */
#define CFA_P70_ACT_MODIFY_DEL_IVLAN 0x4
/** Set to enable Replace or Add of Outer VLAN
 */
#define CFA_P70_ACT_MODIFY_REPL_ADD_OVLAN 0x8
/** Set to enable Replace or Add of Inner VLAN
 */
#define CFA_P70_ACT_MODIFY_REPL_ADD_IVLAN 0x10
/** Set to enable Modify of TTL
 */
#define CFA_P70_ACT_MODIFY_TTL_UPDATE 0x20
/** Set to enable delete of INT Tunnel
 */
#define CFA_P70_ACT_MODIFY_DEL_INT_TNL 0x40
/** For phase 7.0 this bit can be used to modify the
 *  tunnel protocol in addition to deleting internal
 *  or outer tunnel
 */
#define CFA_P70_ACT_MODIFY_TUNNEL_MODIFY CFA_P70_ACT_MODIFY_DEL_INT_TNL
/** Set to enable Modify of Field
 */
#define CFA_P70_ACT_MODIFY_FIELD 0x80
/** Set to enable Modify of Destination MAC
 */
#define CFA_P70_ACT_MODIFY_DMAC 0x100
/** Set to enable Modify of Source MAC
 */
#define CFA_P70_ACT_MODIFY_SMAC 0x200
/** Set to enable Modify of Source IPv6 Address
 */
#define CFA_P70_ACT_MODIFY_SRC_IPV6 0x400
/** Set to enable Modify of Destination IPv6 Address
 */
#define CFA_P70_ACT_MODIFY_DST_IPV6 0x800
/** Set to enable Modify of Source IPv4 Address
 */
#define CFA_P70_ACT_MODIFY_SRC_IPV4 0x1000
/** Set to enable Modify of Destination IPv4 Address
 */
#define CFA_P70_ACT_MODIFY_DST_IPV4 0x2000
/** Set to enable Modify of L4 Source Port
 */
#define CFA_P70_ACT_MODIFY_SRC_PORT 0x4000
/** Set to enable Modify of L4 Destination Port
 */
#define CFA_P70_ACT_MODIFY_DST_PORT 0x8000
	uint16_t modify;

/** Set to enable Modify of KID
 */
#define CFA_P70_ACT_MODIFY_FIELD_KID 0x1
	uint16_t field_modify;

	/* Valid for phase 7.0 or higher */
	enum tunnel_modify_mode_p70 tnl_mod_mode;

	/** Source Control
	 *
	 * Controls the type of the Source Action the template is
	 * describing
	 *
	 * source is valid when:
	 * ((obj_type == CFA_P70_ACT_OBJ_TYPE_FULL_ACT) ||
	 *  (obj_type == CFA_P70_ACT_OBJ_TYPE_COMPACT_ACT) &&
	 *   act.source_enable || act.source_inline)
	 */
	enum source_rec_type_p70 source;
};

/**
 * Key template consists of key fields that can be enabled/disabled
 * individually.
 */
struct cfa_p70_key_template {
	/** [in] Identify if the key template is for TCAM. If false, the
	 *  key template is for EM. This field is mandantory for device that
	 *  only support fix key formats.
	 */
	bool is_wc_tcam_key;
	/** [in] Identify if the key template will be use for IPv6 Keys.
	 *
	 *  Note: This is important for SR2 as the field length for the Flow Id
	 *  is dependent on the L3 flow type.  For SR2 for IPv4 Keys, the Flow
	 *  Id field is 16 bits, for all other types (IPv6, ARP, PTP, EAP, RoCE,
	 *  FCoE, UPAR), the Flow Id field length is 20 bits.
	 */
	bool is_ipv6_key;
	/** [in] key field enable field array, set 1 to the correspeonding
	 *  field enable to make a field valid
	 */
	uint8_t field_en[CFA_P70_KEY_MAX_FIELD_CNT];
};

/**
 * Action template consists of action fields that can be enabled/disabled
 * individually.
 */
struct cfa_p70_action_template {
	/** [in] CFA version for the action template */
	enum cfa_ver hw_ver;
	/** [in] action field enable field array, set 1 to the correspeonding
	 *  field enable to make a field valid
	 */
	uint8_t data[CFA_P70_ACT_MAX_TEMPLATE_SZ];
};

#define CFA_PROF_L2CTXT_TCAM_MAX_FIELD_CNT CFA_P70_PROF_L2_CTXT_TCAM_MAX_FLD
#define CFA_PROF_L2CTXT_REMAP_MAX_FIELD_CNT CFA_P70_PROF_L2_CTXT_RMP_DR_MAX_FLD
#define CFA_PROF_MAX_KEY_CFG_SZ sizeof(struct cfa_p70_prof_key_cfg)

#endif /* _CFA_BLD_P70_H_ */
