find_lan_channel(struct ipmi_intf * intf, uint8_t start)
{
	uint8_t chan = 0;

	for (chan = start; chan < IPMI_CHANNEL_NUMBER_MAX; chan++) {
		if (is_lan_channel(intf, chan)) {
			return chan;
		}
	}
	return 0;
}