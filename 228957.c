print_lan_set_arp_usage(void)
{
	lprintf(LOG_NOTICE,
"lan set <channel> arp respond <on|off>");
	lprintf(LOG_NOTICE,
"lan set <channel> arp generate <on|off>");
	lprintf(LOG_NOTICE,
"lan set <channel> arp interval <seconds>");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"example: lan set 7 arp gratuitous off");
}