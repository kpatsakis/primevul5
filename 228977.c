is_alert_destination(struct ipmi_intf * intf, uint8_t channel, uint8_t alert)
{
	struct lan_param * p;

	p = get_lan_param(intf, channel, IPMI_LANP_NUM_DEST);
	if (!p)
		return 0;
	if (!p->data)
		return 0;

	if (alert <= (p->data[0] & 0xf))
		return 1;
	else
		return 0;
}