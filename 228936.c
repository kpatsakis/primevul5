ipmi_set_alert_enable(struct ipmi_intf *intf, uint8_t channel, uint8_t enable)
{
	struct channel_access_t channel_access;
	int ccode = 0;
	memset(&channel_access, 0, sizeof(channel_access));
	channel_access.channel = channel;
	ccode = _ipmi_get_channel_access(intf, &channel_access, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Get Channel Access(non-volatile) for channel %d",
				channel);
		return (-1);
	}
	if (enable != 0) {
		channel_access.alerting = 1;
	} else {
		channel_access.alerting = 0;
	}
	/* non-volatile */
	ccode = _ipmi_set_channel_access(intf, channel_access, 1, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Set Channel Access(non-volatile) for channel %d",
				channel);
		return (-1);
	}
	/* volatile */
	ccode = _ipmi_set_channel_access(intf, channel_access, 2, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Set Channel Access(volatile) for channel %d",
				channel);
		return (-1);
	}
	printf("PEF alerts for channel %d %s.\n",
			channel,
			(enable) ? "enabled" : "disabled");
	return 0;
}