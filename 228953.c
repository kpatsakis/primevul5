get_cmdline_ipaddr(char * arg, uint8_t * buf)
{
	uint32_t ip1, ip2, ip3, ip4;
	if (sscanf(arg,
				"%" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32,
				&ip1, &ip2, &ip3, &ip4) != 4) {
		lprintf(LOG_ERR, "Invalid IP address: %s", arg);
		return (-1);
	}
	if (ip1 > UINT8_MAX || ip2 > UINT8_MAX
			|| ip3 > UINT8_MAX || ip4 > UINT8_MAX) {
		lprintf(LOG_ERR, "Invalid IP address: %s", arg);
		return (-1);
	}
	buf[0] = (uint8_t)ip1;
	buf[1] = (uint8_t)ip2;
	buf[2] = (uint8_t)ip3;
	buf[3] = (uint8_t)ip4;
	return 0;
}