lan_set_arp_respond(struct ipmi_intf * intf,
		    uint8_t chan, uint8_t ctl)
{
	struct lan_param *lp;
	uint8_t data;

	lp = get_lan_param(intf, chan, IPMI_LANP_BMC_ARP);
	if (!lp)
		return -1;
	if (!lp->data)
		return -1;
	data = lp->data[0];

	/* set arp response bitflag */
	if (ctl == 0)
		data &= ~0x2;
	else
		data |= 0x2;

	printf("%sabling BMC-generated ARP responses\n", ctl ? "En" : "Dis");
	return set_lan_param(intf, chan, IPMI_LANP_BMC_ARP, &data, 1);
}