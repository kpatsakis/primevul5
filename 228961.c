set_lan_param(struct ipmi_intf * intf, uint8_t chan,
	      int param, uint8_t * data, int len)
{
	int rc;
	ipmi_lanp_lock(intf, chan);
	rc = __set_lan_param(intf, chan, param, data, len, 1);
	ipmi_lanp_unlock(intf, chan);
	return rc;
}