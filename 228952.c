print_lan_usage(void)
{
	lprintf(LOG_NOTICE,
"LAN Commands:");
	lprintf(LOG_NOTICE,
"		   print [<channel number>]");
	lprintf(LOG_NOTICE,
"		   set <channel number> <command> <parameter>");
	lprintf(LOG_NOTICE,
"		   alert print <channel number> <alert destination>");
	lprintf(LOG_NOTICE,
"		   alert set <channel number> <alert destination> <command> <parameter>");
	lprintf(LOG_NOTICE,
"		   stats get [<channel number>]");
	lprintf(LOG_NOTICE,
"		   stats clear [<channel number>]");
}