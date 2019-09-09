/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2015 Vincent Bernat <vincent@bernat.im>
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <arpa/inet.h>

#include "lldpctl.h"
#include "../log.h"
#include "atom.h"
#include "helpers.h"

#ifdef ENABLE_DOT3

static lldpctl_map_t port_dot3_power_devicetype_map[] = {
	{ LLDP_DOT3_POWER_PSE, "PSE" },
	{ LLDP_DOT3_POWER_PD,  "PD" },
	{ 0, NULL }
};

static lldpctl_map_t port_dot3_power_pse_source_map[] = {
	{ LLDP_DOT3_POWER_SOURCE_BOTH, "PSE + Local" },
	{ LLDP_DOT3_POWER_SOURCE_PSE, "PSE" },
	{ 0, NULL }
};

static lldpctl_map_t port_dot3_power_pd_source_map[] = {
	{ LLDP_DOT3_POWER_SOURCE_BACKUP, "Backup source" },
	{ LLDP_DOT3_POWER_SOURCE_PRIMARY, "Primary power source" },
	{ 0, NULL }
};

static struct atom_map port_dot3_power_pairs_map = {
	.key = lldpctl_k_dot3_power_pairs,
	.map = {
		{ LLDP_DOT3_POWERPAIRS_SIGNAL, "signal" },
		{ LLDP_DOT3_POWERPAIRS_SPARE,  "spare" },
		{ 0, NULL }
	},
};

static struct atom_map port_dot3_power_class_map = {
	.key = lldpctl_k_dot3_power_class,
	.map = {
		{ 1, "class 0" },
		{ 2, "class 1" },
		{ 3, "class 2" },
		{ 4, "class 3" },
		{ 5, "class 4" },
		{ 0, NULL }
	},
};

static struct atom_map port_dot3_power_4pid_map = {
	.key = lldpctl_k_dot3_power_4pid,
	.map = {
		{ LLDP_DOT3_POWER_4PID_SUP,	"4PID is supported by PD"},
		{ LLDP_DOT3_POWER_4PID_UNSUP,	"4PID is not supported by PD" },
		{ 0, NULL }
	},
};

static struct atom_map port_dot3_power_priority_map = {
	.key = lldpctl_k_dot3_power_priority,
	.map = {
		{ 0,                          "unknown" },
		{ LLDP_MED_POW_PRIO_CRITICAL, "critical" },
		{ LLDP_MED_POW_PRIO_HIGH,     "high" },
		{ LLDP_MED_POW_PRIO_LOW,      "low" },
		{ 0, NULL },
	},
};

/*
static struct atom_map port_dot3_power_pseStatus_map = {
	.key = lldpctl_k_dot3_power_pseStatus ,
	.map = {
		{ LLDP_DOT3_POWER_STATUS_PSE_2PAIR,			"2-pair powering" },
		{ LLDP_DOT3_POWER_STATUS_PSE_4PAIR_SINGLE_SIGNATURE,	"4-pair powering single-signature PD" },
		{ LLDP_DOT3_POWER_STATUS_PSE_4PAIR_DUAL_SIGNATURE,	"4-pair powering dual-signature PD" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_pdStatus_map = {
	.key = lldpctl_k_dot3_power_pdStatus ,
	.map = {
		{ LLDP_DOT3_POWER_STATUS_PD_POWERED_SINGLE_SIGNATURE,	"Powered single-signature PD" },
		{ LLDP_DOT3_POWER_STATUS_PD_2PAIR_DUAL_SIGNATURE,	"2-pair powered dual-signature PD" },
		{ LLDP_DOT3_POWER_STATUS_PD_4PAIR_DUAL_SIGNATURE,	"4-pair powered dual-signature PD" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_pairsExt_map = {
	.key = lldpctl_k_dot3_power_pairsExt ,
	.map = {
		{ LLDP_DOT3_POWERPAIRS_PSE_A,		"Alternative A (signal pairs)" },
		{ LLDP_DOT3_POWERPAIRS_PSE_B,		"Alternative B (spare pairs)" },
		{ LLDP_DOT3_POWERPAIRS_PSE_BOTH,	"Both Alternatives" },
		{ 0, NULL },
	},
};
*/

static struct atom_map port_dot3_power_dualSigAClass_map = {
	.key = lldpctl_k_dot3_power_dualSigAClass ,
	.map = {
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_1,		"Class 1" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_2,		"Class 2" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_3,		"Class 3" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_4,		"Class 4" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_5,		"Class 5" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_SINGLE_SIG_PD,	"Single-signature PD or 2-pair only PSE" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_dualSigBClass_map = {
	.key = lldpctl_k_dot3_power_dualSigBClass ,
	.map = {
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_1,		"Class 1" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_2,		"Class 2" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_3,		"Class 3" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_4,		"Class 4" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_5,		"Class 5" },
		{ LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_SINGLE_SIG_PD,	"Single-signature PD or 2-pair only PSE" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_classExt_map = {
	.key = lldpctl_k_dot3_power_classExt ,
	.map = {
		{ LLDP_DOT3_POWER_CLASS_1,		"Class 1" },
		{ LLDP_DOT3_POWER_CLASS_2,		"Class 2" },
		{ LLDP_DOT3_POWER_CLASS_3,		"Class 3" },
		{ LLDP_DOT3_POWER_CLASS_4,		"Class 4" },
		{ LLDP_DOT3_POWER_CLASS_5,		"Class 5" },
		{ LLDP_DOT3_POWER_CLASS_6,		"Class 6" },
		{ LLDP_DOT3_POWER_CLASS_7,		"Class 7" },
		{ LLDP_DOT3_POWER_CLASS_8,		"Class 8" },
		{ LLDP_DOT3_POWER_CLASS_DUAL_SIG_PD,	"Dual-signature PD" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_powerTypeExt_map = {
	.key = lldpctl_k_dot3_power_powerTypeExt ,
	.map = {
		{ LLDP_DOT3_POWER_TYPE_3_PSE,		"Type 3 PSE" },
		{ LLDP_DOT3_POWER_TYPE_4_PSE,		"Type 4 PSE" },
		{ LLDP_DOT3_POWER_TYPE_3_PD_SINGLE_SIG,	"Type 3 single-signature PD" },
		{ LLDP_DOT3_POWER_TYPE_3_PD_DUAL_SIG,	"Type 3 dual-signature PD" },
		{ LLDP_DOT3_POWER_TYPE_4_PD_SINGLE_SIG,	"Type 4 single-signature PD" },
		{ LLDP_DOT3_POWER_TYPE_4_PD_DUAL_SIG,	"Type 4 dual-signature PD" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_pdLoad_map = {
	.key = lldpctl_k_dot3_power_pdLoad ,
	.map = {
		{ LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_TRUE,
			"PD is dual-signature and power demand on Mode A and Mode B are electrically isolated." },
		{ LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_FALSE,
			"PD is single-signature or dual-signature and power demand on Mode A and Mode B are not electrically isolated." },
		{ 0, NULL },
	},
};

/*
static struct atom_map port_dot3_power_autoclassSupport_map = {
	.key = lldpctl_k_dot3_power_autoclassSupport ,
	.map = {
		{ LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_TRUE,	"PSE supports Autoclass" },
		{ LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_FALSE,	"PSE does not support Autoclass" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_autoclassCompleted_map = {
	.key = lldpctl_k_dot3_power_autoclassCompleted ,
	.map = {
		{ LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_TRUE,	"Autoclass measurement completed" },
		{ LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_IDLE,	"Autoclass idle" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_autoclassRequest_map = {
	.key = lldpctl_k_dot3_power_autoclassRequest ,
	.map = {
		{ LLDP_DOT3_POWER_AUTOCLASS_REQUEST_TRUE,	"PD requests Autoclass measurement" },
		{ LLDP_DOT3_POWER_AUTOCLASS_REQUEST_IDLE,	"Autoclass idle" },
		{ 0, NULL },
	},
};

static struct atom_map port_dot3_power_powerDownRequest_map = {
	.key = lldpctl_k_dot3_power_powerDownRequest ,
	.map = {
		{ LLDP_DOT3_POWER_POWERDOWN_REQUEST,	"PD requests a power down" },
		{ 0, NULL },
	},
};
*/

/*Registering maps allows them to be used in conf-power.c in the register commands functions*/
//TODO, does priority matter? For debugging on heartland, try changing priorities
ATOM_MAP_REGISTER(port_dot3_power_pairs_map,    4);
ATOM_MAP_REGISTER(port_dot3_power_class_map,    5);
ATOM_MAP_REGISTER(port_dot3_power_priority_map, 6);
ATOM_MAP_REGISTER(port_dot3_power_dualSigAClass_map, 7);
ATOM_MAP_REGISTER(port_dot3_power_dualSigBClass_map, 8);
ATOM_MAP_REGISTER(port_dot3_power_classExt_map, 9);
ATOM_MAP_REGISTER(port_dot3_power_powerTypeExt_map, 10);
ATOM_MAP_REGISTER(port_dot3_power_pdLoad_map, 11);

static int
_lldpctl_atom_new_dot3_power(lldpctl_atom_t *atom, va_list ap)
{
	struct _lldpctl_atom_dot3_power_t *dpow =
	    (struct _lldpctl_atom_dot3_power_t *)atom;
	dpow->parent = va_arg(ap, struct _lldpctl_atom_port_t *);
	lldpctl_atom_inc_ref((lldpctl_atom_t *)dpow->parent);
	return 1;
}

static void
_lldpctl_atom_free_dot3_power(lldpctl_atom_t *atom)
{
	struct _lldpctl_atom_dot3_power_t *dpow =
	    (struct _lldpctl_atom_dot3_power_t *)atom;
	lldpctl_atom_dec_ref((lldpctl_atom_t *)dpow->parent);
}

static const char*
_lldpctl_atom_get_str_dot3_power(lldpctl_atom_t *atom, lldpctl_key_t key)
{
	struct _lldpctl_atom_dot3_power_t *dpow =
	    (struct _lldpctl_atom_dot3_power_t *)atom;
	struct lldpd_port     *port     = dpow->parent->port;

	/* Local and remote port */
	switch (key) {
	case lldpctl_k_dot3_power_devicetype:
		return map_lookup(port_dot3_power_devicetype_map,
		    port->p_power.devicetype);
	case lldpctl_k_dot3_power_pairs:
		return map_lookup(port_dot3_power_pairs_map.map,
		    port->p_power.pairs);
	case lldpctl_k_dot3_power_class:
		return map_lookup(port_dot3_power_class_map.map,
		    port->p_power.class);
	case lldpctl_k_dot3_power_source:
		return map_lookup((port->p_power.devicetype == LLDP_DOT3_POWER_PSE)?
		    port_dot3_power_pse_source_map:
		    port_dot3_power_pd_source_map,
		    port->p_power.source);
	case lldpctl_k_dot3_power_priority:
		return map_lookup(port_dot3_power_priority_map.map,
		    port->p_power.priority);
	default:
		SET_ERROR(atom->conn, LLDPCTL_ERR_NOT_EXIST);
		return NULL;
	}
}

static long int
_lldpctl_atom_get_int_dot3_power(lldpctl_atom_t *atom, lldpctl_key_t key)
{
	struct _lldpctl_atom_dot3_power_t *dpow =
	    (struct _lldpctl_atom_dot3_power_t *)atom;
	struct lldpd_port     *port     = dpow->parent->port;

	/* Local and remote port */
	switch (key) {
	case lldpctl_k_dot3_power_devicetype:
		return port->p_power.devicetype;
	case lldpctl_k_dot3_power_supported:
		return port->p_power.supported;
	case lldpctl_k_dot3_power_enabled:
		return port->p_power.enabled;
	case lldpctl_k_dot3_power_paircontrol:
		return port->p_power.paircontrol;
	case lldpctl_k_dot3_power_pairs:
		return port->p_power.pairs;
	case lldpctl_k_dot3_power_class:
		return port->p_power.class;
	case lldpctl_k_dot3_power_type:
		return port->p_power.powertype;
	case lldpctl_k_dot3_power_source:
		return port->p_power.source;
	case lldpctl_k_dot3_power_priority:
		return port->p_power.priority;
	case lldpctl_k_dot3_power_requested:
		return port->p_power.requested * 100;
	case lldpctl_k_dot3_power_allocated:
		return port->p_power.allocated * 100;
	case lldpctl_k_dot3_power_requestedA:
		return port->p_power.requestedA * 100;
	case lldpctl_k_dot3_power_requestedB:
		return port->p_power.requestedB * 100;
	case lldpctl_k_dot3_power_allocatedA:
		return port->p_power.allocatedA * 100;
	case lldpctl_k_dot3_power_allocatedB:
		return port->p_power.allocatedB * 100;
	case lldpctl_k_dot3_power_pseStatus:
		return port->p_power.psePoweringStatus;
	case lldpctl_k_dot3_power_pdStatus:
		return port->p_power.pdPoweredStatus;
	case lldpctl_k_dot3_power_pairsExt:
		return port->p_power.psePowerPairs;
	case lldpctl_k_dot3_power_dualSigAClass:
		return port->p_power.powerClassA;
	case lldpctl_k_dot3_power_dualSigBClass:
		return port->p_power.powerClassB;
	case lldpctl_k_dot3_power_classExt:
		return port->p_power.powerClassExt;
	case lldpctl_k_dot3_power_powerTypeExt:
		return port->p_power.powerTypeExt;
	case lldpctl_k_dot3_power_pdLoad:
		return port->p_power.pdLoad;
	case lldpctl_k_dot3_power_pseMaxPower:
		return port->p_power.pseMaxAvailPower;
	case lldpctl_k_dot3_power_autoclassSupport:
		return port->p_power.pseAutoclassSupport;
	case lldpctl_k_dot3_power_autoclassCompleted:
		return port->p_power.autoClass_completed;
	case lldpctl_k_dot3_power_autoclassRequest:
		return port->p_power.autoClass_request;
	case lldpctl_k_dot3_power_powerDownRequest:
		return port->p_power.powerdown_time;
	case lldpctl_k_dot3_power_powerDownTime:
		return port->p_power.powerdown_request_pd;
	default:
		return SET_ERROR(atom->conn, LLDPCTL_ERR_NOT_EXIST);
	}
}

static lldpctl_atom_t*
_lldpctl_atom_set_int_dot3_power(lldpctl_atom_t *atom, lldpctl_key_t key,
    long int value)
{
	struct _lldpctl_atom_dot3_power_t *dpow =
	    (struct _lldpctl_atom_dot3_power_t *)atom;
	struct lldpd_port *port = dpow->parent->port;

	/* Only local port can be modified */
	if (!dpow->parent->local) {
		SET_ERROR(atom->conn, LLDPCTL_ERR_NOT_EXIST);
		return NULL;
	}

	switch (key) {
	case lldpctl_k_dot3_power_devicetype:
		switch (value) {
		case 0:		/* Disabling */
		case LLDP_DOT3_POWER_PSE:
		case LLDP_DOT3_POWER_PD:
			port->p_power.devicetype = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_supported:
		switch (value) {
		case 0:
		case 1:
			port->p_power.supported = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_enabled:
		switch (value) {
		case 0:
		case 1:
			port->p_power.enabled = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_paircontrol:
		switch (value) {
		case 0:
		case 1:
			port->p_power.paircontrol = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_pairs:
		switch (value) {
		case 1:
		case 2:
			port->p_power.pairs = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_class:
		if (value < 0 || value > 5)
			goto bad;
		port->p_power.class = value;
		return atom;
	case lldpctl_k_dot3_power_type:
		switch (value) {
		case LLDP_DOT3_POWER_8023AT_TYPE1:
		case LLDP_DOT3_POWER_8023AT_TYPE2:
		case LLDP_DOT3_POWER_8023AT_OFF:
			port->p_power.powertype = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_source:
		if (value < 0 || value > 3)
			goto bad;
		port->p_power.source = value;
		return atom;
	case lldpctl_k_dot3_power_4pid:
		switch(value) {
		case LLDP_DOT3_POWER_4PID_SUP:
		case LLDP_DOT3_POWER_4PID_UNSUP:
			port->p_power.pid4 = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_priority:
		switch (value) {
		case LLDP_DOT3_POWER_PRIO_UNKNOWN:
		case LLDP_DOT3_POWER_PRIO_CRITICAL:
		case LLDP_DOT3_POWER_PRIO_HIGH:
		case LLDP_DOT3_POWER_PRIO_LOW:
			port->p_power.priority = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_allocated:
		if (value < 0 || value > 99900) goto bad;
		port->p_power.allocated = value / 100;
		return atom;
	case lldpctl_k_dot3_power_requested:
		if (value < 0 || value > 99900) goto bad;
		port->p_power.requested = value / 100;
		return atom;
	case lldpctl_k_dot3_power_requestedA:
		if (value < 0 || value > 49900) goto bad;
		port->p_power.requestedA = value / 100;
		return atom;
	case lldpctl_k_dot3_power_requestedB:
		if (value < 0 || value > 49900) goto bad;
		port->p_power.requestedB = value / 100;
		return atom;
	case lldpctl_k_dot3_power_allocatedA:
		if (value < 0 || value > 49900) goto bad;
		port->p_power.allocatedA = value / 100;
		return atom;
	case lldpctl_k_dot3_power_allocatedB:
		if (value < 0 || value > 49900) goto bad;
		port->p_power.allocatedB = value / 100;
		return atom;
	case lldpctl_k_dot3_power_pseStatus:
		switch (value) {
		case LLDP_DOT3_POWER_STATUS_PSE_2PAIR:
		case LLDP_DOT3_POWER_STATUS_PSE_4PAIR_SINGLE_SIGNATURE:
		case LLDP_DOT3_POWER_STATUS_PSE_4PAIR_DUAL_SIGNATURE:
			port->p_power.psePoweringStatus = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_pdStatus:
		switch (value) {
		case LLDP_DOT3_POWER_STATUS_PD_POWERED_SINGLE_SIGNATURE:
		case LLDP_DOT3_POWER_STATUS_PD_2PAIR_DUAL_SIGNATURE:
		case LLDP_DOT3_POWER_STATUS_PD_4PAIR_DUAL_SIGNATURE:
			port->p_power.pdPoweredStatus = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_pairsExt:
		switch (value) {
		case LLDP_DOT3_POWERPAIRS_PSE_A:
		case LLDP_DOT3_POWERPAIRS_PSE_B:
		case LLDP_DOT3_POWERPAIRS_PSE_BOTH:
			port->p_power.psePowerPairs = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_dualSigAClass:
		switch (value) {
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_1:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_2:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_3:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_4:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_5:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_SINGLE_SIG_PD:
			port->p_power.powerClassA = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_dualSigBClass:
		switch (value) {
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_1:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_2:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_3:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_4:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_5:
		case LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_SINGLE_SIG_PD:
			port->p_power.powerClassB = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_classExt:
		switch (value) {
		case LLDP_DOT3_POWER_CLASS_1:
		case LLDP_DOT3_POWER_CLASS_2:
		case LLDP_DOT3_POWER_CLASS_3:
		case LLDP_DOT3_POWER_CLASS_4:
		case LLDP_DOT3_POWER_CLASS_5:
		case LLDP_DOT3_POWER_CLASS_6:
		case LLDP_DOT3_POWER_CLASS_7:
		case LLDP_DOT3_POWER_CLASS_8:
		case LLDP_DOT3_POWER_CLASS_DUAL_SIG_PD:
			port->p_power.powerClassExt = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_powerTypeExt:
		switch (value) {
		case LLDP_DOT3_POWER_TYPE_3_PSE:
		case LLDP_DOT3_POWER_TYPE_4_PSE:
		case LLDP_DOT3_POWER_TYPE_3_PD_SINGLE_SIG:
		case LLDP_DOT3_POWER_TYPE_3_PD_DUAL_SIG:
		case LLDP_DOT3_POWER_TYPE_4_PD_SINGLE_SIG:
		case LLDP_DOT3_POWER_TYPE_4_PD_DUAL_SIG:
			port->p_power.powerTypeExt = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_pdLoad:
		switch (value) {
		case LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_TRUE:
		case LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_FALSE:
			port->p_power.pdLoad = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_pseMaxPower:
		if(value < 100 || value > 99900) goto bad;
		port->p_power.pseMaxAvailPower = value / 100;
		return atom;
	case lldpctl_k_dot3_power_autoclassSupport:
		switch (value) {
		case LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_TRUE:
		case LLDP_DOT3_POWER_AUTOCLASS_PSE_SUPPORT_FALSE:
			port->p_power.pseAutoclassSupport = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_autoclassCompleted:
		switch (value) {
		case LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_TRUE:
		case LLDP_DOT3_POWER_AUTOCLASS_COMPLETED_IDLE:
			port->p_power.autoClass_completed = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_autoclassRequest:
		switch (value) {
		case LLDP_DOT3_POWER_AUTOCLASS_REQUEST_TRUE:
		case LLDP_DOT3_POWER_AUTOCLASS_REQUEST_IDLE:
			port->p_power.autoClass_request = value;
			return atom;
		default: goto bad;
		}
	case lldpctl_k_dot3_power_powerDownRequest:
		/*All values are valid, only 0x1D will power it down, everything else ignored*/
		port->p_power.powerdown_request_pd = value;
	case lldpctl_k_dot3_power_powerDownTime:
		/*magic number is 2^18, the number of bits available for power down time */
		if(value < 0 || value > 262143) goto bad;
		port->p_power.powerdown_time = value;
		return atom;
	default:
		SET_ERROR(atom->conn, LLDPCTL_ERR_NOT_EXIST);
		return NULL;
	}

	return atom;
bad:
	SET_ERROR(atom->conn, LLDPCTL_ERR_BAD_VALUE);
	return NULL;
}

/*This function sets the power atom fields.  Key defines which field.  Value is
the string, which corresponds to a map defined above. WARNING: it is case insensitive,
so commands can be registered as something different that what the map displays.
Ex: the device-type field is registered (and stored in env) as "pd", but the map is "PD"*/

static lldpctl_atom_t*
_lldpctl_atom_set_str_dot3_power(lldpctl_atom_t *atom, lldpctl_key_t key,
    const char *value)
{
	switch (key) {
	case lldpctl_k_dot3_power_devicetype:
		return _lldpctl_atom_set_int_dot3_power(atom, key,
		    map_reverse_lookup(port_dot3_power_devicetype_map, value));
	case lldpctl_k_dot3_power_pairs:
		return _lldpctl_atom_set_int_dot3_power(atom, key,
		    map_reverse_lookup(port_dot3_power_pairs_map.map, value));
	case lldpctl_k_dot3_power_class:
		return _lldpctl_atom_set_int_dot3_power(atom, key,
		    map_reverse_lookup(port_dot3_power_class_map.map, value));
	case lldpctl_k_dot3_power_priority:
		return _lldpctl_atom_set_int_dot3_power(atom, key,
		    map_reverse_lookup(port_dot3_power_priority_map.map, value));
	case lldpctl_k_dot3_power_4pid:
		return _lldpctl_atom_set_int_dot3_power(atom, key,
		    map_reverse_lookup(port_dot3_power_4pid_map.map, value));
	default:
		SET_ERROR(atom->conn, LLDPCTL_ERR_NOT_EXIST);
		return NULL;
	}
}

static struct atom_builder dot3_power =
	{ atom_dot3_power, sizeof(struct _lldpctl_atom_dot3_power_t),
	  .init = _lldpctl_atom_new_dot3_power,
	  .free = _lldpctl_atom_free_dot3_power,
	  .get_int = _lldpctl_atom_get_int_dot3_power,
	  .set_int = _lldpctl_atom_set_int_dot3_power,
	  .get_str = _lldpctl_atom_get_str_dot3_power,
	  .set_str = _lldpctl_atom_set_str_dot3_power };

ATOM_BUILDER_REGISTER(dot3_power, 8);

#endif

