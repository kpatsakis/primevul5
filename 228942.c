ipmi_lan_stats_clear(struct ipmi_intf * intf, uint8_t chan)
{
	int rc = 0;
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t msg_data[2];

	if (!is_lan_channel(intf, chan)) {
		lprintf(LOG_ERR, "Channel %d is not a LAN channel", chan);
		return -1;
	}

	/* From here, we are ready to get the stats */
	msg_data[0] = chan;
	msg_data[1] = 1;   /* Clear */

	memset(&req, 0, sizeof(req));
	req.msg.netfn    = IPMI_NETFN_TRANSPORT;
	req.msg.cmd      = IPMI_LAN_GET_STAT;
	req.msg.data     = msg_data;
	req.msg.data_len = 2;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		lprintf(LOG_INFO, "Get LAN Stats command failed");
		return (-1);
	}

	if (rsp->ccode) {
		lprintf(LOG_INFO, "Get LAN Stats command failed: %s",
			val2str(rsp->ccode, completion_code_vals));
		return (-1);
	}

	return rc;
}