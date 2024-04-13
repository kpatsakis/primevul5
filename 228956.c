ipmi_lanp_unlock(struct ipmi_intf * intf, uint8_t chan)
{
	uint8_t val = IPMI_LANP_WRITE_COMMIT;
	int rc;

	rc = __set_lan_param(intf, chan, IPMI_LANP_SET_IN_PROGRESS, &val, 1, 0);
	if (rc < 0) {
		lprintf(LOG_DEBUG, "LAN Parameter Commit not supported");
	}

	val = IPMI_LANP_WRITE_UNLOCK;
	__set_lan_param(intf, chan, IPMI_LANP_SET_IN_PROGRESS, &val, 1, 0);
}