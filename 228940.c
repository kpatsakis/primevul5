is_lan_channel(struct ipmi_intf * intf, uint8_t chan)
{
	uint8_t medium;

	if (chan < 1 || chan > IPMI_CHANNEL_NUMBER_MAX)
		return 0;

	medium = ipmi_get_channel_medium(intf, chan);

	if (medium == IPMI_CHANNEL_MEDIUM_LAN ||
	    medium == IPMI_CHANNEL_MEDIUM_LAN_OTHER)
		return 1;

	return 0;
}