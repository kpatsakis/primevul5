ipmi_lan_alert_print_all(struct ipmi_intf * intf, uint8_t channel)
{
	int j, ndest;
	struct lan_param * p;

	p = get_lan_param(intf, channel, IPMI_LANP_NUM_DEST);
	if (!p)
		return -1;
	if (!p->data)
		return -1;
	ndest = p->data[0] & 0xf;

	for (j=0; j<=ndest; j++) {
		ipmi_lan_alert_print(intf, channel, j);
	}

	return 0;
}