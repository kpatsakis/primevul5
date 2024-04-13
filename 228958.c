get_lan_param_select(struct ipmi_intf * intf, uint8_t chan, int param, int select)
{
	struct lan_param * p = NULL;
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	int i = 0;
	uint8_t msg_data[4];

	for (i = 0; ipmi_lan_params[i].cmd != (-1); i++) {
		if (ipmi_lan_params[i].cmd == param) {
			p = &ipmi_lan_params[i];
			break;
		}
	}

	if (!p) {
		lprintf(LOG_INFO, "Get LAN Parameter failed: Unknown parameter.");
		return NULL;
	}

	msg_data[0] = chan;
	msg_data[1] = p->cmd;
	msg_data[2] = select;
	msg_data[3] = 0;

	memset(&req, 0, sizeof(req));
	req.msg.netfn    = IPMI_NETFN_TRANSPORT;
	req.msg.cmd      = IPMI_LAN_GET_CONFIG;
	req.msg.data     = msg_data;
	req.msg.data_len = 4;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		lprintf(LOG_INFO, "Get LAN Parameter '%s' command failed", p->desc);
		return NULL;
	}

	switch (rsp->ccode)
	{
	case 0x00: /* successful */
		break;

	case 0x80: /* parameter not supported */
	case 0xc9: /* parameter out of range */
	case 0xcc: /* invalid data field in request */

		/* these completion codes usually mean parameter not supported */
		lprintf(LOG_INFO, "Get LAN Parameter '%s' command failed: %s",
			p->desc, val2str(rsp->ccode, completion_code_vals));
		p->data = NULL;
		p->data_len = 0;
		return p;

	default:

		/* other completion codes are treated as error */
		lprintf(LOG_INFO, "Get LAN Parameter '%s' command failed: %s",
			p->desc, val2str(rsp->ccode, completion_code_vals));
		return NULL;
	}

	p->data = rsp->data + 1;
	p->data_len = rsp->data_len - 1;

	return p;
}