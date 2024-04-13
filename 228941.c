ipmi_lanp_lock(struct ipmi_intf * intf, uint8_t chan)
{
	uint8_t val = IPMI_LANP_WRITE_LOCK;
	int retry = 3;

	for (;;) {
		int state = ipmi_lanp_lock_state(intf, chan);
		if (state == -1)
			break;
		if (state == val)
			break;
		if (retry-- == 0)
			break;
		__set_lan_param(intf, chan, IPMI_LANP_SET_IN_PROGRESS,
				&val, 1, 0);
	}
}