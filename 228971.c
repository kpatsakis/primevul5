ipmi_lan_stats_get(struct ipmi_intf * intf, uint8_t chan)
{
	int rc = 0;
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t msg_data[2];
	uint16_t statsTemp;

	if (!is_lan_channel(intf, chan)) {
		lprintf(LOG_ERR, "Channel %d is not a LAN channel", chan);
		return -1;
	}

	/* From here, we are ready to get the stats */

	msg_data[0] = chan;
	msg_data[1] = 0;   /* Don't clear */

	memset(&req, 0, sizeof(req));
	req.msg.netfn    = IPMI_NETFN_TRANSPORT;
	req.msg.cmd      = IPMI_LAN_GET_STAT;
	req.msg.data     = msg_data;
	req.msg.data_len = 2;

	rsp = intf->sendrecv(intf, &req);
	if (!rsp) {
		lprintf(LOG_ERR, "Get LAN Stats command failed");
		return (-1);
	}

	if (rsp->ccode) {
		lprintf(LOG_ERR, "Get LAN Stats command failed: %s",
			val2str(rsp->ccode, completion_code_vals));
		return (-1);
	}

	if (verbose > 1) {
		uint8_t counter;
		printf("--- Rx Stats ---\n");
		for (counter=0; counter<18; counter+=2) {
			printf("%02X", *(rsp->data + counter));
			printf(" %02X - ", *(rsp->data + counter+1));
		}
		printf("\n");
	}

	statsTemp = ((*(rsp->data + 0)) << 8) | (*(rsp->data + 1));
	printf("IP Rx Packet              : %d\n", statsTemp);

	statsTemp = ((*(rsp->data + 2)) << 8) | (*(rsp->data + 3));
	printf("IP Rx Header Errors       : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 4)) << 8) | (*(rsp->data + 5));
	printf("IP Rx Address Errors      : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 6)) << 8) | (*(rsp->data + 7));
	printf("IP Rx Fragmented          : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 8)) << 8) | (*(rsp->data + 9));
	printf("IP Tx Packet              : %u\n", statsTemp);

	statsTemp = ((*(rsp->data +10)) << 8) | (*(rsp->data +11));
	printf("UDP Rx Packet             : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 12)) << 8) | (*(rsp->data + 13));
	printf("RMCP Rx Valid             : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 14)) << 8) | (*(rsp->data + 15));
	printf("UDP Proxy Packet Received : %u\n", statsTemp);

	statsTemp = ((*(rsp->data + 16)) << 8) | (*(rsp->data + 17));
	printf("UDP Proxy Packet Dropped  : %u\n", statsTemp);

	return rc;
}