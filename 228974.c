print_lan_set_ipsrc_usage(void)
{
	lprintf(LOG_NOTICE,
"lan set <channel> ipsrc <source>");
	lprintf(LOG_NOTICE,
"  none   = unspecified");
	lprintf(LOG_NOTICE,
"  static = static address (manually configured)");
	lprintf(LOG_NOTICE,
"  dhcp   = address obtained by BMC running DHCP");
	lprintf(LOG_NOTICE,
"  bios   = address loaded by BIOS or system software");
}