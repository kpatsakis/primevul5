lan_set_arp_interval(struct ipmi_intf * intf, uint8_t chan, uint8_t ival)
{
	struct lan_param *lp;
	uint8_t interval = 0;
	int rc = 0;

	lp = get_lan_param(intf, chan, IPMI_LANP_GRAT_ARP);
	if (!lp)
		return -1;
	if (!lp->data)
		return -1;

	if (ival != 0) {
		if (((UINT8_MAX - 1) / 2) < ival) {
			lprintf(LOG_ERR, "Given ARP interval '%u' is too big.", ival);
			return (-1);
		}
		interval = (ival * 2) - 1;
		rc = set_lan_param(intf, chan, IPMI_LANP_GRAT_ARP, &interval, 1);
	} else {
		interval = lp->data[0];
	}

	printf("BMC-generated Gratuitous ARP interval:  %.1f seconds\n",
	       (float)((interval + 1) / 2));

	return rc;
}