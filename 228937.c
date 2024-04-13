set_lan_param_wait(struct ipmi_intf * intf, uint8_t chan,
		   int param, uint8_t * data, int len)
{
	struct lan_param * p;
	int retry = 10;		/* 10 retries */

	lprintf(LOG_DEBUG, "Waiting for Set LAN Parameter to complete...");
	if (verbose > 1)
		printbuf(data, len, "SET DATA");

	for (;;) {
		p = get_lan_param(intf, chan, param);
		if (!p) {
			sleep(IPMI_LANP_TIMEOUT);
			if (retry-- == 0)
				return -1;
			continue;
		}
		if (verbose > 1)
			printbuf(p->data, p->data_len, "READ DATA");
		if (p->data_len != len) {
			sleep(IPMI_LANP_TIMEOUT);
			if (retry-- == 0) {
				lprintf(LOG_WARNING, "Mismatched data lengths: %d != %d",
				       p->data_len, len);
				return -1;
			}
			continue;
		}
		if (memcmp(data, p->data, len) != 0) {
			sleep(IPMI_LANP_TIMEOUT);
			if (retry-- == 0) {
				lprintf(LOG_WARNING, "LAN Parameter Data does not match!  "
				       "Write may have failed.");
				return -1;
			}
			continue;
		}
		break;
	}
	return 0;
}