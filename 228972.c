print_lan_alert_set_usage(void)
{
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"usage: lan alert set <channel number> <alert destination> <command> <parameter>");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"    Command/parameter options:");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"    ipaddr <x.x.x.x>               Set alert IP address");
	lprintf(LOG_NOTICE,
"    macaddr <x:x:x:x:x:x>          Set alert MAC address");
	lprintf(LOG_NOTICE,
"    gateway <default|backup>       Set channel gateway to use for alerts");
	lprintf(LOG_NOTICE,
"    ack <on|off>                   Set Alert Acknowledge on or off");
	lprintf(LOG_NOTICE,
"    type <pet|oem1|oem2>           Set destination type as PET or OEM");
	lprintf(LOG_NOTICE,
"    time <seconds>                 Set ack timeout or unack retry interval");
	lprintf(LOG_NOTICE,
"    retry <number>                 Set number of alert retries");
	lprintf(LOG_NOTICE,
"");
}