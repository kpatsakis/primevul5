__set_lan_param(struct ipmi_intf * intf, uint8_t chan,
		int param, uint8_t * data, int len, int wait)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t msg_data[32];

	if (param < 0)
		return -1;

	msg_data[0] = chan;
	msg_data[1] = param;

	memcpy(&msg_data[2], data, len);
	memset(&req, 0, sizeof(req));
	req.msg.netfn = IPMI_NETFN_TRANSPORT;
	req.msg.cmd = IPMI_LAN_SET_CONFIG;
	req.msg.data = msg_data;
	req.msg.data_len = len+2;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		lprintf(LOG_ERR, "Set LAN Parameter failed");
		return -1;
	}
	if (rsp->ccode && wait) {
		lprintf(LOG_DEBUG, "Warning: Set LAN Parameter failed: %s",
			val2str(rsp->ccode, completion_code_vals));
		if (rsp->ccode == 0xcc) {
			/* retry hack for invalid data field ccode */
			int retry = 10;		/* 10 retries */
			lprintf(LOG_DEBUG, "Retrying...");
			for (;;) {
				if (retry-- == 0)
					break;
				sleep(IPMI_LANP_TIMEOUT);
				rsp = intf->sendrecv(intf, &req);
				if (!rsp || rsp->ccode)
					continue;
				return set_lan_param_wait(intf, chan, param, data, len);
			}
		}
		else if (rsp->ccode != 0xff) {
			/* let 0xff ccode continue */
			return -1;
		}
	}

	if (!wait)
		return 0;
	return set_lan_param_wait(intf, chan, param, data, len);
}