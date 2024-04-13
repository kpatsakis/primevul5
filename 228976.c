ipmi_lan_set(struct ipmi_intf * intf, int argc, char ** argv)
{
	uint8_t data[32];
	uint8_t chan;
	int rc = 0;

	if (argc < 2) {
		print_lan_set_usage();
		return (-1);
	}

	if (strncmp(argv[0], "help", 4) == 0 ||
	    strncmp(argv[1], "help", 4) == 0) {
		print_lan_set_usage();
		return 0;
	}
	
	if (str2uchar(argv[0], &chan) != 0) {
		lprintf(LOG_ERR, "Invalid channel: %s", argv[0]);
		return (-1);
	}

	/* find type of channel and only accept 802.3 LAN */
	if (!is_lan_channel(intf, chan)) {
		lprintf(LOG_ERR, "Channel %d is not a LAN channel!", chan);
		print_lan_set_usage();
		return -1;
	}

	memset(&data, 0, sizeof(data));

	/* set user access */
	if (strncmp(argv[1], "user", 4) == 0) {
		rc = ipmi_set_user_access(intf, chan, 1);
	}
	/* set channel access mode */
	else if (strncmp(argv[1], "access", 6) == 0) {
		if (argc < 3) {
			print_lan_set_access_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_access_usage();
			return 0;
		}
		else if (strncmp(argv[2], "on", 2) == 0) {
			rc = ipmi_set_channel_access(intf, chan, 1);
		}
		else if (strncmp(argv[2], "off", 3) == 0) {
			rc = ipmi_set_channel_access(intf, chan, 0);
		}
		else {
			print_lan_set_access_usage();
			return (-1);
		}
	}
	/* set ARP control */
	else if (strncmp(argv[1], "arp", 3) == 0) {
		if (argc < 3) {
			print_lan_set_arp_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_arp_usage();
		}
		else if (strncmp(argv[2], "interval", 8) == 0) {
			uint8_t interval = 0;
			if (str2uchar(argv[3], &interval) != 0) {
				lprintf(LOG_ERR, "Given ARP interval '%s' is invalid.", argv[3]);
				return (-1);
			}
			rc = lan_set_arp_interval(intf, chan, interval);
		}
		else if (strncmp(argv[2], "generate", 8) == 0) {
			if (argc < 4) {
				print_lan_set_arp_usage();
				return (-1);
			}
			else if (strncmp(argv[3], "on", 2) == 0)
				rc = lan_set_arp_generate(intf, chan, 1);
			else if (strncmp(argv[3], "off", 3) == 0)
				rc = lan_set_arp_generate(intf, chan, 0);
			else {
				print_lan_set_arp_usage();
				return (-1);
			}
		}
		else if (strncmp(argv[2], "respond", 7) == 0) {
			if (argc < 4) {
				print_lan_set_arp_usage();
				return (-1);
			}
			else if (strncmp(argv[3], "on", 2) == 0)
				rc = lan_set_arp_respond(intf, chan, 1);
			else if (strncmp(argv[3], "off", 3) == 0)
				rc = lan_set_arp_respond(intf, chan, 0);
			else {
				print_lan_set_arp_usage();
				return (-1);
			}
		}
		else {
			print_lan_set_arp_usage();
		}
	}
	/* set authentication types */
	else if (strncmp(argv[1], "auth", 4) == 0) {
		if (argc < 3) {
			print_lan_set_auth_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_auth_usage();
			return 0;
		} else {
			rc = ipmi_lan_set_auth(intf, chan, argv[2], argv[3]);
		}
	}
	/* ip address source */
	else if (strncmp(argv[1], "ipsrc", 5) == 0) {
		if (argc < 3) {
			print_lan_set_ipsrc_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_ipsrc_usage();
			return 0;
		}
		else if (strncmp(argv[2], "none", 4) == 0)
			data[0] = 0;
		else if (strncmp(argv[2], "static", 5) == 0)
			data[0] = 1;
		else if (strncmp(argv[2], "dhcp", 4) == 0)
			data[0] = 2;
		else if (strncmp(argv[2], "bios", 4) == 0)
			data[0] = 3;
		else {
			print_lan_set_ipsrc_usage();
			return -1;
		}
		rc = set_lan_param(intf, chan, IPMI_LANP_IP_ADDR_SRC, data, 1);
	}
	/* session password
	 * not strictly a lan setting, but its used for lan connections */
	else if (strncmp(argv[1], "password", 8) == 0) {
		rc = ipmi_lan_set_password(intf, 1, argv[2]);
	}
	/* snmp community string */
	else if (strncmp(argv[1], "snmp", 4) == 0) {
		if (argc < 3) {
			print_lan_set_snmp_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_snmp_usage();
			return 0;
		} else {
			memcpy(data, argv[2], __min(strlen(argv[2]), 18));
			printf("Setting LAN %s to %s\n",
			       ipmi_lan_params[IPMI_LANP_SNMP_STRING].desc, data);
			rc = set_lan_param(intf, chan, IPMI_LANP_SNMP_STRING, data, 18);
		}
	}
	/* ip address */
	else if (strncmp(argv[1], "ipaddr", 6) == 0) {
		if(argc != 3)
		{
			print_lan_set_usage();
			return -1;
		}
		rc = get_cmdline_ipaddr(argv[2], data);
		if (rc == 0) {
			printf("Setting LAN %s to %d.%d.%d.%d\n",
				ipmi_lan_params[IPMI_LANP_IP_ADDR].desc,
				data[0], data[1], data[2], data[3]);
			rc = set_lan_param(intf, chan, IPMI_LANP_IP_ADDR, data, 4);
		}
	}
	/* network mask */
	else if (strncmp(argv[1], "netmask", 7) == 0) {
		if(argc != 3)
		{
			print_lan_set_usage();
			return -1;
		}
		rc = get_cmdline_ipaddr(argv[2], data);
		if (rc == 0) {
			printf("Setting LAN %s to %d.%d.%d.%d\n",
		       		ipmi_lan_params[IPMI_LANP_SUBNET_MASK].desc,
		       		data[0], data[1], data[2], data[3]);
			rc = set_lan_param(intf, chan, IPMI_LANP_SUBNET_MASK, data, 4);
		}
	}
	/* mac address */
	else if (strncmp(argv[1], "macaddr", 7) == 0) {
		if(argc != 3)
		{
			print_lan_set_usage();
			return -1;
		}
		rc = str2mac(argv[2], data);
		if (rc == 0) {
			printf("Setting LAN %s to %s\n",
				ipmi_lan_params[IPMI_LANP_MAC_ADDR].desc,
				mac2str(data));
			rc = set_lan_param(intf, chan, IPMI_LANP_MAC_ADDR, data, 6);
		}
	}
	/* default gateway settings */
	else if (strncmp(argv[1], "defgw", 5) == 0) {
		if (argc < 4) {
			print_lan_set_defgw_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_defgw_usage();
			return 0;
		}
		else if ((strncmp(argv[2], "ipaddr", 5) == 0) &&
			 (get_cmdline_ipaddr(argv[3], data) == 0)) {
			printf("Setting LAN %s to %d.%d.%d.%d\n",
			       ipmi_lan_params[IPMI_LANP_DEF_GATEWAY_IP].desc,
			       data[0], data[1], data[2], data[3]);
			rc = set_lan_param(intf, chan, IPMI_LANP_DEF_GATEWAY_IP, data, 4);
		}
		else if ((strncmp(argv[2], "macaddr", 7) == 0) &&
			 (str2mac(argv[3], data) == 0)) {
			printf("Setting LAN %s to %s\n",
				ipmi_lan_params[IPMI_LANP_DEF_GATEWAY_MAC].desc,
				mac2str(data));
			rc = set_lan_param(intf, chan, IPMI_LANP_DEF_GATEWAY_MAC, data, 6);
		}
		else {
			print_lan_set_usage();
			return -1;
		}
	}
	/* backup gateway settings */
	else if (strncmp(argv[1], "bakgw", 5) == 0) {
		if (argc < 4) {
			print_lan_set_bakgw_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_bakgw_usage();
			return 0;
		}
		else if ((strncmp(argv[2], "ipaddr", 5) == 0) &&
			 (get_cmdline_ipaddr(argv[3], data) == 0)) {
			printf("Setting LAN %s to %d.%d.%d.%d\n",
			       ipmi_lan_params[IPMI_LANP_BAK_GATEWAY_IP].desc,
			       data[0], data[1], data[2], data[3]);
			rc = set_lan_param(intf, chan, IPMI_LANP_BAK_GATEWAY_IP, data, 4);
		}
		else if ((strncmp(argv[2], "macaddr", 7) == 0) &&
			 (str2mac(argv[3], data) == 0)) {
			printf("Setting LAN %s to %s\n",
				ipmi_lan_params[IPMI_LANP_BAK_GATEWAY_MAC].desc,
				mac2str(data));
			rc = set_lan_param(intf, chan, IPMI_LANP_BAK_GATEWAY_MAC, data, 6);
		}
		else {
			print_lan_set_usage();
			return -1;
		}
	}
	else if (strncasecmp(argv[1], "vlan", 4) == 0) {
		if (argc < 4) {
			print_lan_set_vlan_usage();
			return (-1);
		}
		else if (strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_vlan_usage();
			return 0;
		}
		else if (strncasecmp(argv[2], "id", 2) == 0) {
			if (strncasecmp(argv[3], "off", 3) == 0) {
				ipmi_lan_set_vlan_id(intf, chan, NULL);
			}
			else {
				ipmi_lan_set_vlan_id(intf, chan, argv[3]);
			}
		}
		else if (strncasecmp(argv[2], "priority", 8) == 0) {
			ipmi_lan_set_vlan_priority(intf, chan, argv[3]);
		}
		else {
			print_lan_set_vlan_usage();
			return (-1);
		}
	}
	/* set PEF alerting on or off */
	else if (strncasecmp(argv[1], "alert", 5) == 0) {
		if (argc < 3) {
			lprintf(LOG_NOTICE, "LAN set alert must be 'on' or 'off'");
			return (-1);
		}
		else if (strncasecmp(argv[2], "on", 2) == 0 ||
			 strncasecmp(argv[2], "enable", 6) == 0) {
			printf("Enabling PEF alerts for LAN channel %d\n", chan);
			rc = ipmi_set_alert_enable(intf, chan, 1);
		}
		else if (strncasecmp(argv[2], "off", 3) == 0 ||
			 strncasecmp(argv[2], "disable", 7) == 0) {
			printf("Disabling PEF alerts for LAN channel %d\n", chan);
			rc = ipmi_set_alert_enable(intf, chan, 0);
		}
		else {
			lprintf(LOG_NOTICE, "LAN set alert must be 'on' or 'off'");
			return 0;
		}
	}
	/* RMCP+ cipher suite privilege levels */
	else if (strncmp(argv[1], "cipher_privs", 12) == 0)
	{
		if (argc != 3) {
			print_lan_set_cipher_privs_usage();
			return (-1);
		}
		else if ((strncmp(argv[2], "help", 4) == 0) ||
		    get_cmdline_cipher_suite_priv_data(argv[2], data))
		{
			print_lan_set_cipher_privs_usage();
			return 0;
		}
		else
		{
			rc = set_lan_param(intf, chan, IPMI_LANP_RMCP_PRIV_LEVELS, data, 9);
		}
	}
	else if (strncmp(argv[1], "bad_pass_thresh", 15) == 0)
	{
		if (argc == 3 && strncmp(argv[2], "help", 4) == 0) {
			print_lan_set_bad_pass_thresh_usage();
			return 0;
		}
		if (argc < 6 || get_cmdline_bad_pass_thresh(&argv[2], data)) {
			print_lan_set_bad_pass_thresh_usage();
			return (-1);
		}
		rc = set_lan_param(intf, chan, IPMI_LANP_BAD_PASS_THRESH, data, 6);
	}
	else {
		print_lan_set_usage();
		return (-1);
	}

	return rc;
}