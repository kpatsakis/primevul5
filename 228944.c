ipmi_lan_set_vlan_id(struct ipmi_intf *intf,  uint8_t chan, char *string)
{
	struct lan_param *p;
	uint8_t data[2];
	int rc = -1;

	if (!string) { /* request to disable VLAN */
		lprintf(LOG_DEBUG, "Get current VLAN ID from BMC.");
		p = get_lan_param(intf, chan, IPMI_LANP_VLAN_ID);
		if (p && p->data && p->data_len > 1) {
			int id = ((p->data[1] & 0x0f) << 8) + p->data[0];
			if (IPMI_LANP_VLAN_DISABLE == id) {
				printf("VLAN is already disabled for channel %"
				       PRIu8 "\n", chan);
				rc = 0;
				goto out;
			}
			if (!IPMI_LANP_IS_VLAN_VALID(id)) {
				lprintf(LOG_ERR,
				        "Retrieved VLAN ID %i is out of "
				        "range <%d..%d>.",
				        id,
				        IPMI_LANP_VLAN_ID_MIN,
				        IPMI_LANP_VLAN_ID_MAX);
				goto out;
			}
			data[0] = p->data[0];
			data[1] = p->data[1] & 0x0F;
		} else {
			data[0] = 0;
			data[1] = 0;
		}
	}
	else {
		int id = 0;
		if (str2int(string, &id) != 0) {
			lprintf(LOG_ERR,
			        "Given VLAN ID '%s' is invalid.",
			        string);
			goto out;
		}

		if (!IPMI_LANP_IS_VLAN_VALID(id)) {
			lprintf(LOG_NOTICE,
			        "VLAN ID must be between %d and %d.",
			        IPMI_LANP_VLAN_ID_MIN,
			        IPMI_LANP_VLAN_ID_MAX);
			goto out;
		}
		else {
			data[0] = (uint8_t)id;
			data[1] = (uint8_t)(id >> 8) | 0x80;
		}
	}
	rc = set_lan_param(intf, chan, IPMI_LANP_VLAN_ID, data, 2);

out:
	return rc;
}