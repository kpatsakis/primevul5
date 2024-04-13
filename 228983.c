ipmi_lan_set_vlan_priority(struct ipmi_intf *intf,  uint8_t chan, char *string)
{
	uint8_t data;
	int rc;
	int priority = 0;
	if (str2int(string, &priority) != 0) {
		lprintf(LOG_ERR, "Given VLAN priority '%s' is invalid.", string);
		return (-1);
	}

	if (priority < 0 || priority > 7) {
		lprintf(LOG_NOTICE, "VLAN priority must be between 0 and 7.");
		return (-1);
	}
	data = (uint8_t)priority;
	rc = set_lan_param(intf, chan, IPMI_LANP_VLAN_PRIORITY, &data, 1);
	return rc;
}