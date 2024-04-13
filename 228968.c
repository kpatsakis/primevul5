get_lan_param(struct ipmi_intf * intf, uint8_t chan, int param)
{
	return get_lan_param_select(intf, chan, param, 0);
}