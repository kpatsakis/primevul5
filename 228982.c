print_lan_set_usage(void)
{
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"usage: lan set <channel> <command> <parameter>");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"LAN set command/parameter options:");
	lprintf(LOG_NOTICE,
"  ipaddr <x.x.x.x>               Set channel IP address");
	lprintf(LOG_NOTICE,
"  netmask <x.x.x.x>              Set channel IP netmask");
	lprintf(LOG_NOTICE,
"  macaddr <x:x:x:x:x:x>          Set channel MAC address");
	lprintf(LOG_NOTICE,
"  defgw ipaddr <x.x.x.x>         Set default gateway IP address");
	lprintf(LOG_NOTICE,
"  defgw macaddr <x:x:x:x:x:x>    Set default gateway MAC address");
	lprintf(LOG_NOTICE,
"  bakgw ipaddr <x.x.x.x>         Set backup gateway IP address");
	lprintf(LOG_NOTICE,
"  bakgw macaddr <x:x:x:x:x:x>    Set backup gateway MAC address");
	lprintf(LOG_NOTICE,
"  password <password>            Set session password for this channel");
	lprintf(LOG_NOTICE,
"  snmp <community string>        Set SNMP public community string");
	lprintf(LOG_NOTICE,
"  user                           Enable default user for this channel");
	lprintf(LOG_NOTICE,
"  access <on|off>                Enable or disable access to this channel");
	lprintf(LOG_NOTICE,
"  alert <on|off>                 Enable or disable PEF alerting for this channel");
	lprintf(LOG_NOTICE,
"  arp respond <on|off>           Enable or disable BMC ARP responding");
	lprintf(LOG_NOTICE,
"  arp generate <on|off>          Enable or disable BMC gratuitous ARP generation");
	lprintf(LOG_NOTICE,
"  arp interval <seconds>         Set gratuitous ARP generation interval");
	lprintf(LOG_NOTICE,
"  vlan id <off|<id>>             Disable or enable VLAN and set ID (1-4094)");
	lprintf(LOG_NOTICE,
"  vlan priority <priority>       Set vlan priority (0-7)");
	lprintf(LOG_NOTICE,
"  auth <level> <type,..>         Set channel authentication types");
	lprintf(LOG_NOTICE,
"    level  = CALLBACK, USER, OPERATOR, ADMIN");
	lprintf(LOG_NOTICE,
"    type   = NONE, MD2, MD5, PASSWORD, OEM");
	lprintf(LOG_NOTICE,
"  ipsrc <source>                 Set IP Address source");
	lprintf(LOG_NOTICE,
"    none   = unspecified source");
	lprintf(LOG_NOTICE,
"    static = address manually configured to be static");
	lprintf(LOG_NOTICE,
"    dhcp   = address obtained by BMC running DHCP");
	lprintf(LOG_NOTICE,
"    bios   = address loaded by BIOS or system software");
	lprintf(LOG_NOTICE,
"  cipher_privs XXXXXXXXXXXXXXX   Set RMCP+ cipher suite privilege levels");
	lprintf(LOG_NOTICE,
"    X = Cipher Suite Unused");
	lprintf(LOG_NOTICE,
"    c = CALLBACK");
	lprintf(LOG_NOTICE,
"    u = USER");
	lprintf(LOG_NOTICE,
"    o = OPERATOR");
	lprintf(LOG_NOTICE,
"    a = ADMIN");
	lprintf(LOG_NOTICE,
"    O = OEM");
	lprintf(LOG_NOTICE,
"");
	lprintf(LOG_NOTICE,
"  bad_pass_thresh <thresh_num> <1|0> <reset_interval> <lockout_interval>\n"
"                                Set bad password threshold");
}