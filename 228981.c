ipmi_lanp_lock_state(struct ipmi_intf * intf, uint8_t chan)
{
	struct lan_param * p;
	p = get_lan_param(intf, chan, IPMI_LANP_SET_IN_PROGRESS);
	if (!p)
		return -1;
	if (!p->data)
		return -1;
	return (p->data[0] & 3);
}