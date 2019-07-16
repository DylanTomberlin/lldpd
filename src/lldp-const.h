/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2008 Vincent Bernat <bernat@luffy.cx>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LLDP_H
#define _LLDP_H

/* Definitions prefixed by `LLDP_` are constants from LLDP
 * specifications. Definitions prefixed by `LLDPD_` are custom
 * constants that are useful in the context of lldpd and its clients.
 */

#define LLDP_TLV_ORG			127
#define LLDP_TLV_ORG_OUI_LEN		3
#define LLDP_TLV_ORG_OUI_INFO_MAXLEN	507

/* Chassis ID subtype */
#define LLDP_CHASSISID_SUBTYPE_CHASSIS	1
#define LLDP_CHASSISID_SUBTYPE_IFALIAS	2
#define LLDP_CHASSISID_SUBTYPE_PORT	3
#define LLDP_CHASSISID_SUBTYPE_LLADDR	4
#define LLDP_CHASSISID_SUBTYPE_ADDR	5
#define LLDP_CHASSISID_SUBTYPE_IFNAME	6
#define LLDP_CHASSISID_SUBTYPE_LOCAL	7

/* Port ID subtype */
#define LLDP_PORTID_SUBTYPE_UNKNOWN	0
#define LLDP_PORTID_SUBTYPE_IFALIAS	1
#define LLDP_PORTID_SUBTYPE_PORT	2
#define LLDP_PORTID_SUBTYPE_LLADDR	3
#define LLDP_PORTID_SUBTYPE_ADDR	4
#define LLDP_PORTID_SUBTYPE_IFNAME	5
#define LLDP_PORTID_SUBTYPE_AGENTCID	6
#define LLDP_PORTID_SUBTYPE_LOCAL	7
#define LLDP_PORTID_SUBTYPE_MAX		LLDP_PORTID_SUBTYPE_LOCAL

/* Operational MAU Type field. See: https://www.iana.org/assignments/ianamau-mib/ianamau-mib */
#define LLDP_DOT3_MAU_AUI		1
#define LLDP_DOT3_MAU_10BASE5		2
#define LLDP_DOT3_MAU_FOIRL		3
#define LLDP_DOT3_MAU_10BASE2		4
#define LLDP_DOT3_MAU_10BASET		5
#define LLDP_DOT3_MAU_10BASEFP		6
#define LLDP_DOT3_MAU_10BASEFB		7
#define LLDP_DOT3_MAU_10BASEFL		8
#define LLDP_DOT3_MAU_10BROAD36		9
#define LLDP_DOT3_MAU_10BASETHD		10
#define LLDP_DOT3_MAU_10BASETFD		11
#define LLDP_DOT3_MAU_10BASEFLHD	12
#define LLDP_DOT3_MAU_10BASEFLFD	13
#define LLDP_DOT3_MAU_100BASET4		14
#define LLDP_DOT3_MAU_100BASETXHD	15
#define LLDP_DOT3_MAU_100BASETXFD	16
#define LLDP_DOT3_MAU_100BASEFXHD	17
#define LLDP_DOT3_MAU_100BASEFXFD	18
#define LLDP_DOT3_MAU_100BASET2HD	19
#define LLDP_DOT3_MAU_100BASET2FD	20
#define LLDP_DOT3_MAU_1000BASEXHD	21
#define LLDP_DOT3_MAU_1000BASEXFD	22
#define LLDP_DOT3_MAU_1000BASELXHD	23
#define LLDP_DOT3_MAU_1000BASELXFD	24
#define LLDP_DOT3_MAU_1000BASESXHD	25
#define LLDP_DOT3_MAU_1000BASESXFD	26
#define LLDP_DOT3_MAU_1000BASECXHD	27
#define LLDP_DOT3_MAU_1000BASECXFD	28
#define LLDP_DOT3_MAU_1000BASETHD	29
#define LLDP_DOT3_MAU_1000BASETFD	30
#define LLDP_DOT3_MAU_10GIGBASEX	31
#define LLDP_DOT3_MAU_10GIGBASELX4	32
#define LLDP_DOT3_MAU_10GIGBASER	33
#define LLDP_DOT3_MAU_10GIGBASEER	34
#define LLDP_DOT3_MAU_10GIGBASELR	35
#define LLDP_DOT3_MAU_10GIGBASESR	36
#define LLDP_DOT3_MAU_10GIGBASEW	37
#define LLDP_DOT3_MAU_10GIGBASEEW	38
#define LLDP_DOT3_MAU_10GIGBASELW	39
#define LLDP_DOT3_MAU_10GIGBASESW	40
#define LLDP_DOT3_MAU_10GIGBASECX4	41
#define LLDP_DOT3_MAU_2BASETL		42
#define LLDP_DOT3_MAU_10PASSTS		43
#define LLDP_DOT3_MAU_100BASEBX10D	44
#define LLDP_DOT3_MAU_100BASEBX10U	45
#define LLDP_DOT3_MAU_100BASELX10	46
#define LLDP_DOT3_MAU_1000BASEBX10D	47
#define LLDP_DOT3_MAU_1000BASEBX10U	48
#define LLDP_DOT3_MAU_1000BASELX10	49
#define LLDP_DOT3_MAU_1000BASEPX10D	50
#define LLDP_DOT3_MAU_1000BASEPX10U	51
#define LLDP_DOT3_MAU_1000BASEPX20D	52
#define LLDP_DOT3_MAU_1000BASEPX20U	53
#define LLDP_DOT3_MAU_10GBASET		54
#define LLDP_DOT3_MAU_10GBASELRM	55
#define LLDP_DOT3_MAU_1000BASEKX	56
#define LLDP_DOT3_MAU_10GBASEKX4	57
#define LLDP_DOT3_MAU_10GBASEKR		58
#define LLDP_DOT3_MAU_10G1GBASEPRXD1	59
#define LLDP_DOT3_MAU_10G1GBASEPRXD2	60
#define LLDP_DOT3_MAU_10G1GBASEPRXD3	61
#define LLDP_DOT3_MAU_10G1GBASEPRXU1	62
#define LLDP_DOT3_MAU_10G1GBASEPRXU2	63
#define LLDP_DOT3_MAU_10G1GBASEPRXU3	64
#define LLDP_DOT3_MAU_10GBASEPRD1	65
#define LLDP_DOT3_MAU_10GBASEPRD2	66
#define LLDP_DOT3_MAU_10GBASEPRD3	67
#define LLDP_DOT3_MAU_10GBASEPRU1	68
#define LLDP_DOT3_MAU_10GBASEPRU3	69
#define LLDP_DOT3_MAU_40GBASEKR4	70
#define LLDP_DOT3_MAU_40GBASECR4	71
#define LLDP_DOT3_MAU_40GBASESR4	72
#define LLDP_DOT3_MAU_40GBASEFR		73
#define LLDP_DOT3_MAU_40GBASELR4	74
#define LLDP_DOT3_MAU_100GBASECR10	75
#define LLDP_DOT3_MAU_100GBASESR10	76
#define LLDP_DOT3_MAU_100GBASELR4	77
#define LLDP_DOT3_MAU_100GBASEER4	78

/* Dot3 Power Devicetype */
#define LLDP_DOT3_POWER_PSE	1
#define LLDP_DOT3_POWER_PD	2

/* Dot3 Power Pairs (RFC 3621) */
#define LLDP_DOT3_POWERPAIRS_SIGNAL	1
#define LLDP_DOT3_POWERPAIRS_SPARE	2

/* Dot3 Power type (for 802.3at) */
#define LLDP_DOT3_POWER_8023AT_OFF	0
#define LLDP_DOT3_POWER_8023AT_TYPE1	1
#define LLDP_DOT3_POWER_8023AT_TYPE2	2

/* Dot3 power source */
#define LLDP_DOT3_POWER_SOURCE_UNKNOWN	0
#define LLDP_DOT3_POWER_SOURCE_PRIMARY	1
#define LLDP_DOT3_POWER_SOURCE_PSE	1
#define LLDP_DOT3_POWER_SOURCE_BACKUP	2
#define LLDP_DOT3_POWER_SOURCE_LOCAL	2
#define LLDP_DOT3_POWER_SOURCE_BOTH	3

/* Dot3 PD 4PID (aka can PD be powered on both modes simultaneously?) */
//TODO work this into the existing bit shift logic in lldp.c
#define LLDP_DOT3_POWER_DUAL_MODE_SUP	1
#define LLDP_DOT3_POWER_DUAL_MODE_UNSUP	0

/* Dot3 power priority */
#define LLDP_DOT3_POWER_PRIO_UNKNOWN	0
#define LLDP_DOT3_POWER_PRIO_CRITICAL	1
#define LLDP_DOT3_POWER_PRIO_HIGH	2
#define LLDP_DOT3_POWER_PRIO_LOW	3

/* Dot3 pse powering status (for 802.3bt) */
#define LLDP_DOT3_POWER_STATUS_PSE_2PAIR			1
#define LLDP_DOT3_POWER_STATUS_PSE_4PAIR_SINGLE_SIGNATURE 	2
#define LLDP_DOT3_POWER_STATUS_PSE_4PAIR_DUAL_SIGNATURE 	3

/* Dot3 pd powered status (802.3bt)*/
#define LLDP_DOT3_POWER_STATUS_PD_POWERED_SINGLE_SIGNATURE	1
#define LLDP_DOT3_POWER_STATUS_PD_2PAIR_DUAL_SIGNAUTRE		2
#define LLDP_DOT3_POWER_STATUS_PD_4PAIR_DUAL_SIGNAUTRE		3

/* Dot3 PSE power pairs ext (802.3bt)*/
#define LLDP_DOT3_POWERPAIRS_PSE_A	1
#define LLDP_DOT3_POWERPAIRS_PSE_B	2
#define LLDP_DOT3_POWERPAIRS_PSE_BOTH	3

/* Dot3 Dual-signature power class ext mode A (802.3bt)*/

#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_1		1
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_2		2
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_3		3
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_4		4
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_5		5
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_SINGLE_SIG_PD	7

/* Dot3 Dual-signature power class ext mode B (802.3bt)*/
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_1		1
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_2		2
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_3		3
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_4		4
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_5		5
#define LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_SINGLE_SIG_PD	7

/* Dot3 Power class ext (802.3bt)*/
#define LLDP_DOT3_POWER_CLASS_1			1
#define LLDP_DOT3_POWER_CLASS_2			2
#define LLDP_DOT3_POWER_CLASS_3			3
#define LLDP_DOT3_POWER_CLASS_4			4
#define LLDP_DOT3_POWER_CLASS_5			5
#define LLDP_DOT3_POWER_CLASS_6			6
#define LLDP_DOT3_POWER_CLASS_7			7
#define LLDP_DOT3_POWER_CLASS_8			8
#define LLDP_DOT3_POWER_CLASS_DUAL_SIG_PD	15

/* Dot3 System Setup, PD load (802.3bt)*/
#define LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_TRUE	1 /*dual signature*/
#define LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_FALSE	0 /*dual or single signature*/

/* Dot3 System Setup, Power type ext (802.3bt)*/
#define LLDP_DOT3_POWER_TYPE_BTOFF		-1
#define LLDP_DOT3_POWER_TYPE_3_PSE		0
#define LLDP_DOT3_POWER_TYPE_4_PSE		1
#define LLDP_DOT3_POWER_TYPE_3_PD_SINGLE_SIG	2
#define LLDP_DOT3_POWER_TYPE_3_PD_DUAL_SIG	3
#define LLDP_DOT3_POWER_TYPE_4_PD_SINGLE_SIG	4
#define LLDP_DOT3_POWER_TYPE_4_PD_DUAL_SIG	5

/* Dot3 Autoclass, request(802.3bt) */
#define LLDP_DOT3_POWER_AUTOCLASS_REQUEST_IDLE	0
#define LLDP_DOT3_POWER_AUTOCLASS_REQUEST_TRUE	1

/* Dot3 Autoclass, completed (802.3bt) */
#define LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_IDLE	0
#define LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_TRUE	1

/* Dot3 Autoclass, PSE Autoclass support (802.3bt) */
#define LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_FALSE	0
#define LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_TRUE	1

/* Dot3 Powerdown (802.3bt) */
#define LLDP_DOT3_POWER_POWERDOWN_LEN		3

#define LLDP_DOT3_POWER_POWERDOWN_REQUEST	0x1D

/* Dot3 POE Measurement TLV (802.3bt)*/
#define LLDP_DOT3_MEASUREMENT_ENERGY_MEAS_VALID		1
#define LLDP_DOT3_MEASUREMENT_ENERGY_MEAS_DISABLED	0

#define LLDP_DOT3_MEASUREMENT_POWER_MEAS_VALID		1
#define LLDP_DOT3_MEASUREMENT_POWER_MEAS_DISABLED	0

#define LLDP_DOT3_MEASUREMENT_CURRENT_MEAS_VALIED	1
#define LLDP_DOT3_MEASUREMENT_CURRENT_MEAS_DISABLED	0

#define LLDP_DOT3_MEASUREMENT_VOLTAGE_MEAS_VALID	1
#define LLDP_DOT3_MEASUREMENT_VOLTAGE_MEAS_DISABLED	0

#define LLDP_DOT3_MEASUREMENT_ENERGY_REQUEST_TRUE	1
#define LLDP_DOT3_MEASUREMENT_ENERGY_REQUEST_FLASE	0

#define LLDP_DOT3_MEASUREMENT_POWER_REQUEST_TRUE	1
#define LLDP_DOT3_MEASUREMENT_POWER_REQUEST_FALSE	0

#define LLDP_DOT3_MEASUREMENT_CURRENT_REQUEST_TRUE	1
#define LLDP_DOT3_MEASUREMENT_CURRENT_REQUEST_FALSE	0

#define LLDP_DOT3_MEASUREMENT_VOLTAGE_REQUEST_TRUE	1
#define LLDP_DOT3_MEASUREMENT_VOLTAGE_REQUEST_FALSE	0

#define LLDP_DOT3_MEASUREMENT_SOURCE_NO_REQUEST		0
#define LLDP_DOT3_MEASUREMENT_SOURCE_PAIRSET_A		1
#define LLDP_DOT3_MEASUREMENT_SOURCE_PAIRSET_B		2
#define LLDP_DOT3_MEASUREMENT_SOURCE_PORT_TOTAL		3

#define LLDP_DOT3_MEASUREMENT_ENERGY_SUPPORT_TRUE	1
#define LLDP_DOT3_MEASUREMENT_ENERGY_SUPPORT_FALSE	0

#define LLDP_DOT3_MEASUREMENT_POWER_SUPPORT_TRUE	1
#define LLDP_DOT3_MEASUREMENT_POWER_SUPPORT_FALSE	0

#define LLDP_DOT3_MEASUREMENT_CURRENT_SUPPORT_TRUE	1
#define LLDP_DOT3_MEASUREMENT_CURRENT_SUPPORT_FALSE	0

#define LLDP_DOT3_MEASUREMENT_VOLTAGE_SUPPORT_TRUE	1
#define LLDP_DOT3_MEASUREMENT_VOLTAGE_SUPPORT_FALSE	0


/* PMD Auto-Negotiation Advertised Capability field, from RFC 3636 (see
 * IANAifMauAutoNegCapBits). Unfortunately, we are limited to two bytes, so
 * higher speed capabilities will map to "other". */
#define LLDP_DOT3_LINK_AUTONEG_OTHER		0x8000
#define LLDP_DOT3_LINK_AUTONEG_10BASE_T		0x4000
#define LLDP_DOT3_LINK_AUTONEG_10BASET_FD	0x2000
#define LLDP_DOT3_LINK_AUTONEG_100BASE_T4	0x1000
#define LLDP_DOT3_LINK_AUTONEG_100BASE_TX	0x0800
#define LLDP_DOT3_LINK_AUTONEG_100BASE_TXFD	0x0400
#define LLDP_DOT3_LINK_AUTONEG_100BASE_T2	0x0200
#define LLDP_DOT3_LINK_AUTONEG_100BASE_T2FD	0x0100
#define LLDP_DOT3_LINK_AUTONEG_FDX_PAUSE	0x0080
#define LLDP_DOT3_LINK_AUTONEG_FDX_APAUSE	0x0040
#define LLDP_DOT3_LINK_AUTONEG_FDX_SPAUSE	0x0020
#define LLDP_DOT3_LINK_AUTONEG_FDX_BPAUSE	0x0010
#define LLDP_DOT3_LINK_AUTONEG_1000BASE_X	0x0008
#define LLDP_DOT3_LINK_AUTONEG_1000BASE_XFD	0x0004
#define LLDP_DOT3_LINK_AUTONEG_1000BASE_T	0x0002
#define LLDP_DOT3_LINK_AUTONEG_1000BASE_TFD	0x0001

/* Capabilities */
#define LLDP_CAP_OTHER		0x01
#define LLDP_CAP_REPEATER	0x02
#define LLDP_CAP_BRIDGE		0x04
#define LLDP_CAP_WLAN		0x08
#define LLDP_CAP_ROUTER		0x10
#define LLDP_CAP_TELEPHONE	0x20
#define LLDP_CAP_DOCSIS		0x40
#define LLDP_CAP_STATION	0x80

#define LLDP_PPVID_CAP_SUPPORTED	(1 << 1)
#define LLDP_PPVID_CAP_ENABLED		(1 << 2)

/* see http://www.iana.org/assignments/address-family-numbers */
#define LLDP_MGMT_ADDR_NONE	0
#define LLDP_MGMT_ADDR_IP4	1
#define LLDP_MGMT_ADDR_IP6	2

#define LLDP_MGMT_IFACE_UNKNOWN 1
#define LLDP_MGMT_IFACE_IFINDEX 2
#define LLDP_MGMT_IFACE_SYSPORT	3

#define LLDP_MED_CLASS_I	1
#define LLDP_MED_CLASS_II	2
#define LLDP_MED_CLASS_III	3
#define LLDP_MED_NETWORK_DEVICE	4

/* LLDP MED application ttpes */
#define LLDP_MED_APPTYPE_UNDEFINED		0
#define LLDP_MED_APPTYPE_VOICE			1
#define LLDP_MED_APPTYPE_VOICESIGNAL		2
#define LLDP_MED_APPTYPE_GUESTVOICE		3
#define LLDP_MED_APPTYPE_GUESTVOICESIGNAL	4
#define LLDP_MED_APPTYPE_SOFTPHONEVOICE		5
#define LLDP_MED_APPTYPE_VIDEOCONFERENCE	6
#define LLDP_MED_APPTYPE_VIDEOSTREAM		7
#define LLDP_MED_APPTYPE_VIDEOSIGNAL		8
#define LLDP_MED_APPTYPE_LAST			LLDP_MED_APPTYPE_VIDEOSIGNAL

/* LLDP MED location formats */
#define LLDP_MED_LOCFORMAT_COORD	1
#define LLDP_MED_LOCFORMAT_CIVIC	2
#define LLDP_MED_LOCFORMAT_ELIN		3
#define LLDP_MED_LOCFORMAT_LAST		LLDP_MED_LOCFORMAT_ELIN

#define LLDP_MED_LOCATION_GEOID_WGS84		1
#define LLDP_MED_LOCATION_GEOID_NAD83		2
#define LLDP_MED_LOCATION_GEOID_NAD83_MLLW	3

#define LLDP_MED_LOCATION_ALTITUDE_UNIT_METER	1
#define LLDP_MED_LOCATION_ALTITUDE_UNIT_FLOOR	2

/* LLDP MED power related constants */
#define LLDP_MED_POW_TYPE_PSE		1
#define LLDP_MED_POW_TYPE_PD		2
#define LLDP_MED_POW_TYPE_RESERVED	3

#define LLDP_MED_POW_SOURCE_UNKNOWN	1
#define LLDP_MED_POW_SOURCE_PRIMARY	2
#define LLDP_MED_POW_SOURCE_BACKUP	3
#define LLDP_MED_POW_SOURCE_RESERVED	4
#define LLDP_MED_POW_SOURCE_PSE		5
#define LLDP_MED_POW_SOURCE_LOCAL	6
#define LLDP_MED_POW_SOURCE_BOTH	7

#define LLDP_MED_POW_PRIO_UNKNOWN	0
#define LLDP_MED_POW_PRIO_CRITICAL	1
#define LLDP_MED_POW_PRIO_HIGH		2
#define LLDP_MED_POW_PRIO_LOW		3

/* LLDP MED capabilities */
#define LLDP_MED_CAP_CAP	0x01
#define LLDP_MED_CAP_POLICY	0x02
#define LLDP_MED_CAP_LOCATION	0x04
#define LLDP_MED_CAP_MDI_PSE	0x08
#define LLDP_MED_CAP_MDI_PD	0x10
#define LLDP_MED_CAP_IV		0x20

/* Protocol constants for multi-protocol lldpd */
#define LLDPD_MODE_LLDP		1
#define LLDPD_MODE_CDPV1	2
#define LLDPD_MODE_CDPV2	3
#define LLDPD_MODE_SONMP	4
#define LLDPD_MODE_EDP		5
#define LLDPD_MODE_FDP		6
#define LLDPD_MODE_MAX		LLDPD_MODE_FDP


/* Bond slave src mac type constants */
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_UNKNOWN	0
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_REAL	1
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_ZERO	2
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_FIXED	3
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_LOCALLY_ADMINISTERED 4
#define LLDP_BOND_SLAVE_SRC_MAC_TYPE_MAX LLDP_BOND_SLAVE_SRC_MAC_TYPE_LOCALLY_ADMINISTERED

/* Agent types */
#define LLDP_AGENT_TYPE_UNKNOWN			0
#define LLDP_AGENT_TYPE_NEAREST_BRIDGE		1
#define LLDP_AGENT_TYPE_NEAREST_NONTPMR_BRIDGE	2
#define LLDP_AGENT_TYPE_NEAREST_CUSTOMER_BRIDGE	3
#define LLDP_AGENT_TYPE_MAX			LLDP_AGENT_TYPE_NEAREST_CUSTOMER_BRIDGE

#endif /* _LLDP_H */
