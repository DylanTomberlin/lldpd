/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2013 Vincent Bernat <bernat@luffy.cx>
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

#include <unistd.h>
#include <string.h>

#include "client.h"
#include "../log.h"

static int
cmd_medpower(struct lldpctl_conn_t *conn, struct writer *w,
    struct cmd_env *env, void *arg)
{
	log_debug("lldpctl", "set MED power");
	lldpctl_atom_t *port;
	const char *name;
	while ((port = cmd_iterate_on_ports(conn, env, &name))) {
		lldpctl_atom_t *med_power;
		const char *what = NULL;

		med_power = lldpctl_atom_get(port, lldpctl_k_port_med_power);
		if (med_power == NULL) {
			log_warnx("lldpctl", "unable to set LLDP-MED power: support seems unavailable");
			continue; /* Need to finish the loop */
		}

		if ((what = "device type", lldpctl_atom_set_str(med_power,
			    lldpctl_k_med_power_type,
			    cmdenv_get(env, "device-type"))) == NULL ||
		    (what = "power source", lldpctl_atom_set_str(med_power,
			lldpctl_k_med_power_source,
			cmdenv_get(env, "source"))) == NULL ||
		    (what = "power priority", lldpctl_atom_set_str(med_power,
			lldpctl_k_med_power_priority,
			cmdenv_get(env, "priority"))) == NULL ||
		    (what = "power value", lldpctl_atom_set_str(med_power,
			lldpctl_k_med_power_val,
			cmdenv_get(env, "value"))) == NULL)
			log_warnx("lldpctl",
			    "unable to set LLDP MED power value for %s on %s. %s.",
			    what, name, lldpctl_last_strerror(conn));
		else {
			if (lldpctl_atom_set(port, lldpctl_k_port_med_power,
				med_power) == NULL) {
				log_warnx("lldpctl", "unable to set LLDP MED power on %s. %s.",
				    name, lldpctl_last_strerror(conn));
			} else
				log_info("lldpctl", "LLDP-MED power has been set for port %s",
				    name);
		}

		lldpctl_atom_dec_ref(med_power);
	}
	return 1;
}

static int
cmd_store_powerpairs_env_value_and_pop2(struct lldpctl_conn_t *conn, struct writer *w,
    struct cmd_env *env, void *value)
{
	return cmd_store_something_env_value_and_pop2("powerpairs", env, value);
}
static int
cmd_store_class_env_value_and_pop2(struct lldpctl_conn_t *conn, struct writer *w,
    struct cmd_env *env, void *value)
{
	return cmd_store_something_env_value_and_pop2("class", env, value);
}
static int
cmd_store_prio_env_value_and_pop2(struct lldpctl_conn_t *conn, struct writer *w,
    struct cmd_env *env, void *value)
{
	return cmd_store_something_env_value_and_pop2("priority", env, value);
}
/*check if bt tlv is single signature*/
static int
cmd_check_single_sig(struct cmd_env *env, void *arg)
{
	const char *typebt = cmdenv_get(env, "typebt");
	if (typebt == NULL) {return 0;}
	return !strcmp(typebt, "3single") || !strcmp(typebt, "4single");
}
/*check if bt tlve is dual signature*/
static int
cmd_check_dual_sig(struct cmd_env *env, void *arg)
{
	const char *typebt = cmdenv_get(env, "typebt");
	if (typebt == NULL) {return 0;}
	return !strcmp(typebt, "3dual") || !strcmp(typebt, "4dual");
}

/*This function takes values stored in the command line environment and
sets the atom values accordingly*/
static int
cmd_dot3power(struct lldpctl_conn_t *conn, struct writer *w,
    struct cmd_env *env, void *arg)
{
	log_debug("lldpctl", "set dot3 power");
	lldpctl_atom_t *port;
	const char *name;
	while ((port = cmd_iterate_on_ports(conn, env, &name))) {
		lldpctl_atom_t *dot3_power;
		const char *what = NULL;
		int ok = 1;

		dot3_power = lldpctl_atom_get(port, lldpctl_k_port_dot3_power);
		if (dot3_power == NULL) {
			log_warnx("lldpctl", "unable to set Dot3 power: support seems unavailable");
			continue; /* Need to finish the loop */
		}

		if (
		    (what = "device type", lldpctl_atom_set_str(dot3_power,
			    lldpctl_k_dot3_power_devicetype,
			    cmdenv_get(env, "device-type"))) == NULL ||
		    /* Flags */
		    (what = "supported flag", lldpctl_atom_set_int(dot3_power,
			lldpctl_k_dot3_power_supported,
			cmdenv_get(env, "supported")?1:0)) == NULL ||
		    (what = "enabled flag", lldpctl_atom_set_int(dot3_power,
			lldpctl_k_dot3_power_enabled,
			cmdenv_get(env, "enabled")?1:0)) == NULL ||
		    (what = "paircontrol flag", lldpctl_atom_set_int(dot3_power,
			lldpctl_k_dot3_power_paircontrol,
			cmdenv_get(env, "paircontrol")?1:0)) == NULL ||
		    /* Powerpairs */
		    (what = "power pairs", lldpctl_atom_set_str(dot3_power,
			lldpctl_k_dot3_power_pairs,
			cmdenv_get(env, "powerpairs"))) == NULL ||
		    /* Class */
		    (what = "power class", cmdenv_get(env, "class")?
			lldpctl_atom_set_str(dot3_power,
			    lldpctl_k_dot3_power_class,
			    cmdenv_get(env, "class")):
			lldpctl_atom_set_int(dot3_power,
			    lldpctl_k_dot3_power_class, 0)) == NULL ||
		    (what = "802.3at type", lldpctl_atom_set_int(dot3_power,
			lldpctl_k_dot3_power_type, 0)) == NULL
		) {
			log_warnx("lldpctl",
			    "unable to set LLDP Dot3 power value for %s on %s. %s.",
			    what, name, lldpctl_last_strerror(conn));
			ok = 0;
		/*802.3at*/
		} else if (cmdenv_get(env, "typeat")) {
			int typeat = cmdenv_get(env, "typeat")[0] - '0';
			const char *source = cmdenv_get(env, "source");
			const char *pid4 = cmdenv_get(env, "pid4");
			if (
			    (what = "802.3at type", lldpctl_atom_set_int(dot3_power,
				    lldpctl_k_dot3_power_type,
				    typeat)) == NULL ||
			    (what = "source", lldpctl_atom_set_int(dot3_power,
				lldpctl_k_dot3_power_source,
				(!strcmp(source, "primary"))?LLDP_DOT3_POWER_SOURCE_PRIMARY:
				(!strcmp(source, "backup"))? LLDP_DOT3_POWER_SOURCE_BACKUP:
				(!strcmp(source, "pse"))?    LLDP_DOT3_POWER_SOURCE_PSE:
				(!strcmp(source, "local"))?  LLDP_DOT3_POWER_SOURCE_LOCAL:
				(!strcmp(source, "both"))?   LLDP_DOT3_POWER_SOURCE_BOTH:
				LLDP_DOT3_POWER_SOURCE_UNKNOWN)) == NULL ||
			    (what = "PD 4 PID", cmdenv_get(env, "pid4")?
				lldpctl_atom_set_str(dot3_power,
				    lldpctl_k_dot3_power_4pid,
				    cmdenv_get(env, "pid4")):
				lldpctl_atom_set_int(dot3_power,
				    lldpctl_k_dot3_power_4pid, 0)) == NULL ||
			    (what = "priority", lldpctl_atom_set_str(dot3_power,
				lldpctl_k_dot3_power_priority,
				cmdenv_get(env, "priority"))) == NULL ||
			    (what = "requested power", lldpctl_atom_set_str(dot3_power,
				lldpctl_k_dot3_power_requested,
				cmdenv_get(env, "requested"))) == NULL ||
			    (what = "allocated power", lldpctl_atom_set_str(dot3_power,
				lldpctl_k_dot3_power_allocated,
				cmdenv_get(env, "allocated"))) == NULL 
			) {
				log_warnx("lldpctl",
				    "unable to set LLDP Dot3 power value for %s on %s. %s.",
				    what, name, lldpctl_last_strerror(conn));
				ok = 0;
			/*802.3bt*/
			} else if (cmdenv_get(env, "pid4")) {
				//add cmdenv_get calls
				const char *pdStatus = cmdenv_get(env, "pdStatus");
				const char *pseStatus = cmdenv_get(env, "pseStatus");
				const char *pairsExt = cmdenv_get(env, "pairsExt");
				const char *aClass = cmdenv_get(env, "aClass");
				const char *bClass = cmdenv_get(env, "bClass");
				const char *classExt = cmdenv_get(env, "classExt");
				const char *typebt = cmdenv_get(env, "typebt");
				const char *pdLoad = cmdenv_get(env, "pdLoad");
				const char *autoclassSupport = cmdenv_get(env, "autoclassSupport");
				const char *autoclassComplete = cmdenv_get(env, "autoclassComplete");
				const char *autoclassRequest = cmdenv_get(env, "autoclassRequest");
				const char *powerDownRequest = cmdenv_get(env, "powerDownRequest");
				if(
				    (what = "requested power A", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_requestedA,
					cmdenv_get(env, "aRequested"))
					|| !cmd_check_dual_sig(env,NULL)) == NULL ||
				    (what = "requested power B", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_requestedB,
					cmdenv_get(env, "bRequested"))
					|| !cmd_check_dual_sig(env,NULL)) == NULL ||
				    (what = "allocated power A", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_allocatedA,
					cmdenv_get(env, "aAllocated"))
					|| !cmd_check_dual_sig(env,NULL)) == NULL ||
				    (what = "allocated power B", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_allocatedB,
					cmdenv_get(env, "bAllocated"))
					|| !cmd_check_dual_sig(env,NULL)) == NULL ||
				/*Power Status field*/
				    (what = "PD status", (pdStatus) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_pdStatus,
					(!strcmp(pdStatus, "single"))?		LLDP_DOT3_POWER_STATUS_PD_POWERED_SINGLE_SIGNATURE:
					(!strcmp(pdStatus, "dual2pair"))?	LLDP_DOT3_POWER_STATUS_PD_2PAIR_DUAL_SIGNATURE:
					(!strcmp(pdStatus, "dual4pair"))?	LLDP_DOT3_POWER_STATUS_PD_4PAIR_DUAL_SIGNATURE:
					0):1) == NULL ||
				    (what = "PSE status", (pseStatus) ?  lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_pseStatus,
					(!strcmp(pseStatus, "2pair"))?		LLDP_DOT3_POWER_STATUS_PSE_2PAIR:
					(!strcmp(pseStatus, "single4pair"))?	LLDP_DOT3_POWER_STATUS_PSE_4PAIR_SINGLE_SIGNATURE:
					(!strcmp(pseStatus, "dual4pair"))?	LLDP_DOT3_POWER_STATUS_PSE_4PAIR_DUAL_SIGNATURE:
					0):1) == NULL ||
				    (what = "pairs extension", pairsExt ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_pairsExt,
					(!strcmp(pairsExt, "both"))?	LLDP_DOT3_POWERPAIRS_PSE_BOTH:
					(!strcmp(pairsExt, "signal"))?	LLDP_DOT3_POWERPAIRS_PSE_A:
					(!strcmp(pairsExt, "spare"))?	LLDP_DOT3_POWERPAIRS_PSE_B:
					0):1) == NULL ||
				    (what = "power class pair A", (aClass) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_dualSigAClass,
					(!strcmp(aClass, "1"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_1:
					(!strcmp(aClass, "2"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_2:
					(!strcmp(aClass, "3"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_3:
					(!strcmp(aClass, "4"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_4:
					(!strcmp(aClass, "5"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_A_CLASS_5:
					0):1) == NULL ||
				    (what = "power class pair B", (bClass) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_dualSigBClass,
					(!strcmp(bClass, "1"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_1:
					(!strcmp(bClass, "2"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_2:
					(!strcmp(bClass, "3"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_3:
					(!strcmp(bClass, "4"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_4:
					(!strcmp(bClass, "5"))?		LLDP_DOT3_POWER_DUAL_SIGNATURE_B_CLASS_5:
					0):1) == NULL ||
				    (what = "power class extension", (classExt) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_classExt,
					(!strcmp(classExt, "1"))?		LLDP_DOT3_POWER_CLASS_1:
					(!strcmp(classExt, "2"))?		LLDP_DOT3_POWER_CLASS_2:
					(!strcmp(classExt, "3"))?		LLDP_DOT3_POWER_CLASS_3:
					(!strcmp(classExt, "4"))?		LLDP_DOT3_POWER_CLASS_4:
					(!strcmp(classExt, "5"))?		LLDP_DOT3_POWER_CLASS_5:
					(!strcmp(classExt, "6"))?		LLDP_DOT3_POWER_CLASS_6:
					(!strcmp(classExt, "7"))?		LLDP_DOT3_POWER_CLASS_7:
					(!strcmp(classExt, "8"))?		LLDP_DOT3_POWER_CLASS_8:
					0):1) == NULL ||

				/*system setup field*/
				    (what = "bt power type extension", (typebt) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_powerTypeExt,
					(!strcmp(typebt, "3single"))?		LLDP_DOT3_POWER_TYPE_3_PD_SINGLE_SIG:
					(!strcmp(typebt, "3dual"))?		LLDP_DOT3_POWER_TYPE_3_PD_DUAL_SIG:
					(!strcmp(typebt, "4single"))?		LLDP_DOT3_POWER_TYPE_4_PD_SINGLE_SIG:
					(!strcmp(typebt, "4dual"))?		LLDP_DOT3_POWER_TYPE_4_PD_DUAL_SIG:
					0):1) == NULL ||
					
				    (what = "pd load", (pdLoad) ? lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_pdLoad,
					(!strcmp(pdLoad, "isolated"))?		LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_TRUE:
					(!strcmp(pdLoad, "not-isolated"))?	LLDP_DOT3_POWER_PD_LOAD_AB_ISOLATION_FALSE:
					0):1) == NULL ||
				/*PSE max power field*/
				    (what = "pse max available power", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_pseMaxPower,
					cmdenv_get(env, "pseMaxPower"))
					|| !strcmp("pd", cmdenv_get(env, "device-type"))) == NULL || /*make non-mandatory for PDs*/
				/*autoclass field*/
				    (what = "auto class support", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_autoclassSupport,
					cmdenv_get(env, "autoclassSupport"))
					|| !strcmp("pd", cmdenv_get(env, "device-type"))) == NULL || /*make non-mandatory for PDs*/
				    (what = "autoclass complete", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_autoclassCompleted,
					cmdenv_get(env, "autoclassCompleted"))
					|| !strcmp("pd", cmdenv_get(env, "device-type"))) == NULL || /*make non-mandatory for PDs*/
		    		    (what = "autoclass request flag", lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_autoclassRequest,
					cmdenv_get(env, "autoclassRequest")?1:0)) == NULL ||
		    		    (what = "power down request flag", lldpctl_atom_set_int(dot3_power,
					lldpctl_k_dot3_power_powerDownRequest,
					cmdenv_get(env, "powerDownRequest")?LLDP_DOT3_POWER_POWERDOWN_REQUEST:0)) == NULL ||
				    (what = "power down time", lldpctl_atom_set_str(dot3_power,
					lldpctl_k_dot3_power_powerDownTime,
					cmdenv_get(env, "powerDownTime"))
					/*make non-mandatory for unless powerdown request flag has been*/
					|| !cmdenv_get(env, "powerDownRequest")) == NULL
				) {
					log_warnx("lldpctl",
					    "unable to set LLDP Dot3 power value for %s on %s. %s.",
					    what, name, lldpctl_last_strerror(conn));
					ok = 0;
				}
			}
		}

		if (ok) {
			if (lldpctl_atom_set(port, lldpctl_k_port_dot3_power,
				dot3_power) == NULL) {
				log_warnx("lldpctl", "unable to set LLDP Dot3 power on %s. %s.",
				    name, lldpctl_last_strerror(conn));
			} else
				log_info("lldpctl", "LLDP Dot3 power has been set for port %s",
				    name);
		}

		lldpctl_atom_dec_ref(dot3_power);
	}//end while
	return 1;
}

static int
cmd_check_type_but_no(struct cmd_env *env, void *arg)
{
	const char *what = arg;
	if (!cmdenv_get(env, "device-type")) return 0;
	if (cmdenv_get(env, what)) return 0;
	return 1;
}
static int
cmd_check_typeat_but_no(struct cmd_env *env, void *arg)
{
	const char *what = arg;
	if (!cmdenv_get(env, "typeat")) return 0;
	if (cmdenv_get(env, what)) return 0;
	return 1;
}
static int
cmd_check_typebt_but_no(struct cmd_env *env, void *arg)
{
	const char *what = arg;
	if (!cmdenv_get(env, "typebt")) return 0;
	if (cmdenv_get(env, what)) return 0;
	return 1;
}
static int
cmd_check_typebt_and_pd_but_no(struct cmd_env *env, void *arg)
{
	const char *devicetype = cmdenv_get(env, "device-type");
	if (devicetype == NULL) {return 0;}
	return cmd_check_typebt_but_no(env, arg) && !strcmp(devicetype, "pd");
}
static int
cmd_check_typebt_and_pse_but_no(struct cmd_env *env, void *arg)
{
	const char *devicetype = cmdenv_get(env, "device-type");
	if (devicetype == NULL) {return 0;}
	return cmd_check_typebt_but_no(env, arg) && !strcmp(devicetype, "pse");
}
static int
cmd_check_type(struct cmd_env *env, const char *type)
{
	const char *etype = cmdenv_get(env, "device-type");
	if (!etype) return 0;
	return (!strcmp(type, etype));
}
static int
cmd_check_pse(struct cmd_env *env, void *arg)
{
	return cmd_check_type(env, "pse");
}
static int
cmd_check_pd(struct cmd_env *env, void *arg)
{
	return cmd_check_type(env, "pd");
}
static int
cmd_check_powerdown_request(struct cmd_env *env, void *arg)
{
	return !!cmdenv_get(env, "powerDownRequest");
}
static void
register_commands_pow_source(struct cmd_node *source)
{
	commands_new(source,
	    "unknown", "Unknown power source",
	    NULL, cmd_store_env_value_and_pop2, "source");
	commands_new(source,
	    "primary", "Primary power source",
	    cmd_check_pse, cmd_store_env_value_and_pop2, "source");
	commands_new(source,
	    "backup", "Backup power source",
	    cmd_check_pse, cmd_store_env_value_and_pop2, "source");
	commands_new(source,
	    "pse", "Power source is PSE",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "source");
	commands_new(source,
	    "local", "Local power source",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "source");
	commands_new(source,
	    "both", "Both PSE and local source available",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "source");
}

static void
register_commands_pow_priority(struct cmd_node *priority, int key)
{
	for (lldpctl_map_t *prio_map =
		 lldpctl_key_get_map(key);
	     prio_map->string;
	     prio_map++) {
		char *tag = strdup(totag(prio_map->string));
		SUPPRESS_LEAK(tag);
		commands_new(
			priority,
			tag,
			prio_map->string,
			NULL, cmd_store_prio_env_value_and_pop2, prio_map->string);
	}
}

/**
 * Register `configure med power` commands.
 */
void
register_commands_medpow(struct cmd_node *configure_med)
{
	struct cmd_node *configure_medpower = commands_new(
		configure_med,
		"power", "MED power configuration",
		NULL, NULL, NULL);

	commands_new(
		configure_medpower,
		NEWLINE, "Apply new MED power configuration",
		cmd_check_env, cmd_medpower, "device-type,source,priority,value");

	/* Type: PSE or PD */
	commands_new(
		configure_medpower,
		"pd", "MED power consumer",
		cmd_check_no_env, cmd_store_env_value_and_pop, "device-type");
	commands_new(
		configure_medpower,
		"pse", "MED power provider",
		cmd_check_no_env, cmd_store_env_value_and_pop, "device-type");

	/* Source */
	struct cmd_node *source = commands_new(
		configure_medpower,
		"source", "MED power source",
		cmd_check_type_but_no, NULL, "source");
	register_commands_pow_source(source);

	/* Priority */
	struct cmd_node *priority = commands_new(
		configure_medpower,
		"priority", "MED power priority",
		cmd_check_type_but_no, NULL, "priority");
	register_commands_pow_priority(priority, lldpctl_k_med_power_priority);

	/* Value */
	commands_new(
		commands_new(configure_medpower,
		    "value", "MED power value",
		    cmd_check_type_but_no, NULL, "value"),
		NULL, "MED power value in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "value");
}

static int
cmd_check_env_power(struct cmd_env *env, void *nothing)
{
	/* We need type and powerpair but if we have typeat, we also request
	 * source, priority, requested and allocated. */
	if (!cmdenv_get(env, "device-type")) return 0;
	if (!cmdenv_get(env, "powerpairs")) return 0;
	if (cmdenv_get(env, "typeat")) {
		return (!!cmdenv_get(env, "source") &&
		    !!cmdenv_get(env, "priority") &&
		    !!cmdenv_get(env, "requested") &&
		    !!cmdenv_get(env, "allocated"));
	}
	return 1;
}

/**
 * Register `configure dot3 power` commands.
 */
void
register_commands_dot3pow(struct cmd_node *configure_dot3)
{
	struct cmd_node *configure_dot3power = commands_new(
		configure_dot3,
		"power", "Dot3 power configuration",
		NULL, NULL, NULL);

	commands_new(
		configure_dot3power,
		NEWLINE, "Apply new Dot3 power configuration",
		cmd_check_env_power, cmd_dot3power, NULL);

	/* Type: PSE or PD */
	commands_new(
		configure_dot3power,
		"pd", "Dot3 power consumer",
		cmd_check_no_env, cmd_store_env_value_and_pop, "device-type");
	commands_new(
		configure_dot3power,
		"pse", "Dot3 power provider",
		cmd_check_no_env, cmd_store_env_value_and_pop, "device-type");

	/* Flags */
	commands_new(
		configure_dot3power,
		"supported", "MDI power support present",
		cmd_check_type_but_no, cmd_store_env_and_pop, "supported");
	commands_new(
		configure_dot3power,
		"enabled", "MDI power support enabled",
		cmd_check_type_but_no, cmd_store_env_and_pop, "enabled");
	commands_new(
		configure_dot3power,
		"paircontrol", "MDI power pair can be selected",
		cmd_check_type_but_no, cmd_store_env_and_pop, "paircontrol");

	/* Power pairs */
	struct cmd_node *powerpairs = commands_new(
		configure_dot3power,
		"powerpairs", "Which pairs are currently used for power (mandatory)",
		cmd_check_type_but_no, NULL, "powerpairs");
	for (lldpctl_map_t *pp_map =
		 lldpctl_key_get_map(lldpctl_k_dot3_power_pairs);
	     pp_map->string;
	     pp_map++) {
		commands_new(
			powerpairs,
			pp_map->string,
			pp_map->string,
			NULL, cmd_store_powerpairs_env_value_and_pop2, pp_map->string);
	}

	/* Class */
	struct cmd_node *class = commands_new(
		configure_dot3power,
		"class", "Power class",
		cmd_check_type_but_no, NULL, "class");
	for (lldpctl_map_t *class_map =
		 lldpctl_key_get_map(lldpctl_k_dot3_power_class);
	     class_map->string;
	     class_map++) {
		const char *tag = strdup(totag(class_map->string));
		SUPPRESS_LEAK(tag);
		commands_new(
			class,
			tag,
			class_map->string,
			NULL, cmd_store_class_env_value_and_pop2, class_map->string);
	}

	/* 802.3at type */
	struct cmd_node *typeat = commands_new(
		configure_dot3power,
		"typeat", "802.3at device type",
		cmd_check_type_but_no, NULL, "typeat");
	commands_new(typeat,
	    "1", "802.3at type 1",
	    NULL, cmd_store_env_value_and_pop2, "typeat");
	commands_new(typeat,
	    "2", "802.3at type 2",
	    NULL, cmd_store_env_value_and_pop2, "typeat");

	/* Source */
	struct cmd_node *source = commands_new(
		configure_dot3power,
		"source", "802.3at dot3 power source (mandatory)",
		cmd_check_typeat_but_no, NULL, "source");
	register_commands_pow_source(source);

	struct cmd_node *pid4 = commands_new(
		configure_dot3power,
		"pid4", "802.3bt pd supports 4 pair power",
		cmd_check_typebt_and_pd_but_no, NULL, "pid4");
	commands_new(
		pid4,
		"supported", "PD supports powering of both modes simultaneously",
		NULL, cmd_store_env_value_and_pop2, "pid4");
	commands_new(pid4,
		"unsupported", "PD does not support powering of both modes simultaneously",
		NULL, cmd_store_env_value_and_pop2, "pid4");
	/* Priority */
	struct cmd_node *priority = commands_new(
		configure_dot3power,
		"priority", "802.3at dot3 power priority (mandatory)",
		cmd_check_typeat_but_no, NULL, "priority");
	register_commands_pow_priority(priority, lldpctl_k_dot3_power_priority);

	/* Values */
	/*Requested*/
	commands_new(
		commands_new(configure_dot3power,
		    "requested", "802.3at dot3 power value requested (mandatory)",
		    cmd_check_typeat_but_no, NULL, "requested"),
		NULL, "802.3at power value requested in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "requested");
	/*Allocated*/
	commands_new(
		commands_new(configure_dot3power,
		    "allocated", "802.3at dot3 power value allocated (mandatory)",
		    cmd_check_typeat_but_no, NULL, "allocated"),
		NULL, "802.3at power value allocated in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "allocated");
	/*Requested A (802.3bt)*/
	commands_new(
		commands_new(configure_dot3power,
		    "aRequested", "802.3bt dot3 power value requested on pair A (mandatory)",
		    cmd_check_dual_sig, NULL, "aRequested"),
		NULL, "802.3bt power value requested on pair A in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "aRequested");
	/*Requested B (802.3bt)*/
	commands_new(
		commands_new(configure_dot3power,
		    "bRequested", "802.3bt dot3 power value requested on pair B (mandatory)",
		    cmd_check_dual_sig, NULL, "bRequested"),
		NULL, "802.3bt power value requested on pair B in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "bRequested");
	/*Allocated A (802.3bt)*/
	commands_new(
		commands_new(configure_dot3power,
		    "aAllocated", "802.3bt dot3 power value allocated on pair A (mandatory)",
		    cmd_check_dual_sig, NULL, "aAllocated"),
		NULL, "802.3bt power value allocated on pair A in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "aAllocated");
	/*Allocated B (802.3bt)*/
	commands_new(
		commands_new(configure_dot3power,
		    "bAllocated", "802.3bt dot3 power value allocated on pair B (mandatory)",
		    cmd_check_dual_sig, NULL, "bAllocated"),
		NULL, "802.3bt power value allocated on pair B in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "bAllocated");

	//TODO, do I need to make it so when this command (and others) has already been
	//traversed in the help menu, it doesn't show up again?
	/* PSE powering status (802.3bt) */
	struct cmd_node *pseStatus = commands_new(
		configure_dot3power,
		"pseStatus", "what signature and pairs of PD is pse powering? (Mandatory)",
		cmd_check_typebt_and_pse_but_no, NULL, "pseStatus");
	commands_new(pseStatus,
		"dual4pair", "4-pair powering dual-signature PD",
		NULL, cmd_store_env_value_and_pop2, "pseStatus");
	commands_new(pseStatus,
		"single4pair", "4-pair powering single-signature PD",
		NULL, cmd_store_env_value_and_pop2, "pseStatus");
	commands_new(pseStatus,
		"2Pair", "2-pair powering",
		NULL, cmd_store_env_value_and_pop2, "pseStatus");

	/*PD powered status (802.3bt)*/
	struct cmd_node *pdStatus = commands_new(
		configure_dot3power,
		"pdStatus", "what signature and pairs of PD? (Mandatory)",
		cmd_check_typebt_and_pd_but_no, NULL, "pdStatus");
	commands_new(pdStatus,
		"dual4pair", "PD is being powered by 4-pairs, dual signature",
		NULL, cmd_store_env_value_and_pop2, "pdStatus");
	commands_new(pdStatus,
		"dual2pair", "PD is being powered by 2-pairs, dual signature",
		NULL, cmd_store_env_value_and_pop2, "pdStatus");
	commands_new(pdStatus,
		"single", "single signature PD",
		NULL, cmd_store_env_value_and_pop2, "pdStatus");
	
	/*PSE power pairs ext*/
	struct cmd_node *pairsExt = commands_new(
		configure_dot3power,
		"pairsExt", "Which pairs are powered? (Mandatory)",
		cmd_check_typebt_and_pse_but_no, NULL, "pairsExt");
	commands_new(pairsExt,
		"both", "Both alternatives powered",
		NULL, cmd_store_env_value_and_pop2, "pairsExt");
	commands_new(pairsExt,
		"signal", "Alternative A powered",
		NULL, cmd_store_env_value_and_pop2, "pairsExt");
	commands_new(pairsExt,
		"spare", "Alternative B powered",
		NULL, cmd_store_env_value_and_pop2, "pairsExt");

	/*Pair A class ext*/
	struct cmd_node *aClass = commands_new(
		configure_dot3power,
		"aClass", "Power class of power on signal pair (alt B)",
		cmd_check_typebt_but_no, NULL, "aClass");
	for (lldpctl_map_t *aClass_map =
		 lldpctl_key_get_map(lldpctl_k_dot3_power_dualSigAClass);
	     aClass_map->string;
	     aClass_map++) {
		const char *tag = strdup(totag(aClass_map->string));
		SUPPRESS_LEAK(tag);
		commands_new(
			aClass,
			tag,
			aClass_map->string,
			NULL, cmd_store_class_env_value_and_pop2, aClass_map->string);
	}

	/*Pair B class ext*/
	struct cmd_node *bClass = commands_new(
		configure_dot3power,
		"bClass", "Power class of power on spare pair (alt B)",
		cmd_check_typebt_but_no, NULL, "bClass");
	for (lldpctl_map_t *bClass_map =
		 lldpctl_key_get_map(lldpctl_k_dot3_power_dualSigBClass);
	     bClass_map->string;
	     bClass_map++) {
		const char *tag = strdup(totag(bClass_map->string));
		SUPPRESS_LEAK(tag);
		commands_new(
			bClass,
			tag,
			bClass_map->string,
			NULL, cmd_store_class_env_value_and_pop2, bClass_map->string);
	}

	/*802.3bt class extension*/
	struct cmd_node *classExt = commands_new(
		configure_dot3power,
		"classExt", "802.3bt Power class extension",
		cmd_check_typebt_but_no, NULL, "classExt");
	for (lldpctl_map_t *classExt_map =
		 lldpctl_key_get_map(lldpctl_k_dot3_power_classExt);
	     classExt_map->string;
	     classExt_map++) {
		const char *tag = strdup(totag(classExt_map->string));
		SUPPRESS_LEAK(tag);
		commands_new(
			classExt,
			tag,
			classExt_map->string,
			NULL, cmd_store_class_env_value_and_pop2, classExt_map->string);
	}

	/* 802.3bt type (power type ext) */
	struct cmd_node *typebt = commands_new(
		configure_dot3power,
		"typebt", "802.3bt type extension",
		cmd_check_typeat_but_no, NULL, "typebt");
	commands_new(typebt,
	    "pse3", "802.3bt type 3 PSE",
	    cmd_check_pse, cmd_store_env_value_and_pop2, "typebt");
	commands_new(typebt,
	    "pse4", "802.3bt type 4 PSE",
	    cmd_check_pse, cmd_store_env_value_and_pop2, "typebt");
	commands_new(typebt,
	    "3single", "802.3bt type 3 single signature PD",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "typebt");
	commands_new(typebt,
	    "3dual", "802.3bt type 3 dual signature PD",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "typebt");
	commands_new(typebt,
	    "4single", "802.3bt type 4 single signature PD",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "typebt");
	commands_new(typebt,
	    "4dual", "802.3bt type 4 dual signature PD",
	    cmd_check_pd, cmd_store_env_value_and_pop2, "typebt");

	/*PD load*/
	struct cmd_node *pdLoad = commands_new(
		configure_dot3power,
		"pdLoad", "4 pair isolation",
		cmd_check_typebt_and_pd_but_no, NULL, "pdLoad");
	commands_new(pdLoad,
		"isolated", "PD is dual-signature and power demand on Mode A and Mode B are electrically isolated.",
		NULL, cmd_store_class_env_value_and_pop2, "pdLoad");
	commands_new(pdLoad,
		"not-isolated", "PD is single-signature or power demand on Mode A and Mode B are not electrically isolated.",
		NULL, cmd_store_class_env_value_and_pop2, "pdLoad");

	/*PSE max avail power*/
	commands_new(
		commands_new(configure_dot3power,
		    "pseMaxPower", "Maximum power a PSE can grant",
		    cmd_check_typebt_and_pse_but_no, NULL, "pseMaxPower"),
		NULL, "Maximum power a PSE can grant in milliwatts",
		NULL, cmd_store_env_value_and_pop2, "pseMaxPower");

	/*Autoclass Flags*/
	commands_new(
		configure_dot3power,
		"autoclassSupport", "PSE supports Autoclass",
		cmd_check_typebt_and_pse_but_no, cmd_store_env_and_pop, "autoclassSupport");
	commands_new(
		configure_dot3power,
		"autoclassComplete", "Autoclass measurement complete if true, idle if false",
		cmd_check_typebt_and_pse_but_no, cmd_store_env_and_pop, "autoclassComplete");
	commands_new(
		configure_dot3power,
		"autoclassRequest", "PD requests autoclass measurement if true, idle if false",
		cmd_check_typebt_and_pd_but_no, cmd_store_env_and_pop, "autoclassRequest");

	/*Power down*/
	commands_new(
		configure_dot3power,
		"powerDownRequest", "PD requests powerdown",
		cmd_check_typebt_and_pd_but_no, cmd_store_env_and_pop, "powerDownRequest");
	commands_new(
		commands_new(configure_dot3power,
		    "powerDownTime", "Time in seconds PD requests power down for",
		    cmd_check_powerdown_request, NULL, "powerDownTime"),
		NULL, "Time in seconds PD requests power down for",
		NULL, cmd_store_env_value_and_pop2, "powerDownTime");
}

