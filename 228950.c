ipmi_set_channel_access(struct ipmi_intf *intf, uint8_t channel,
		uint8_t enable)
{
	struct channel_access_t channel_access;
	int ccode = 0;
	memset(&channel_access, 0, sizeof(channel_access));
	channel_access.channel = channel;
	/* Get Non-Volatile Channel Access first */
	ccode = _ipmi_get_channel_access(intf, &channel_access, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Get Channel Access(non-volatile) for channel %d",
				channel);
		return (-1);
	}

	if (enable != 0) {
		channel_access.access_mode = 2;
	} else {
		channel_access.access_mode = 0;
	}
	channel_access.privilege_limit = 0x04;
	ccode = _ipmi_set_channel_access(intf, channel_access, 1, 1);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Set Channel Access(non-volatile) for channel %d",
				channel);
		return (-1);
	}

	memset(&channel_access, 0, sizeof(channel_access));
	channel_access.channel = channel;
	/* Get Volatile Channel Access */
	ccode = _ipmi_get_channel_access(intf, &channel_access, 1);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Get Channel Access(volatile) for channel %d",
				channel);
		return (-1);
	}

	if (enable != 0) {
		channel_access.access_mode = 2;
	} else {
		channel_access.access_mode = 0;
	}
	channel_access.privilege_limit = 0x04;
	ccode = _ipmi_set_channel_access(intf, channel_access, 2, 2);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Set Channel Access(volatile) for channel %d",
				channel);
		return (-1);
	}

	/* can't send close session if access off so abort instead */
	if (enable == 0) {
		intf->abort = 1;
	}
	printf("Set Channel Access for channel %d was successful.\n",
			channel);
	return 0;
}