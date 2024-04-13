ipmi_lan_print(struct ipmi_intf * intf, uint8_t chan)
{
	struct lan_param * p;

	if (chan < 1 || chan > IPMI_CHANNEL_NUMBER_MAX) {
		lprintf(LOG_ERR, "Invalid Channel %d", chan);
		return -1;
	}

	/* find type of channel and only accept 802.3 LAN */
	if (!is_lan_channel(intf, chan)) {
		lprintf(LOG_ERR, "Channel %d is not a LAN channel", chan);
		return -1;
	}

	p = get_lan_param(intf, chan, IPMI_LANP_SET_IN_PROGRESS);
	if (!p)
		return -1;
	if (p->data) {
		printf("%-24s: ", p->desc);
		p->data[0] &= 3;
		switch (p->data[0]) {
		case 0:
			printf("Set Complete\n");
			break;
		case 1:
			printf("Set In Progress\n");
			break;
		case 2:
			printf("Commit Write\n");
			break;
		case 3:
			printf("Reserved\n");
			break;
		default:
			printf("Unknown\n");
		}
	}

	p = get_lan_param(intf, chan, IPMI_LANP_AUTH_TYPE);
	if (!p)
		return -1;
	if (p->data) {
		printf("%-24s: %s%s%s%s%s\n", p->desc,
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
	}

	p = get_lan_param(intf, chan, IPMI_LANP_AUTH_TYPE_ENABLE);
	if (!p)
		return -1;
	if (p->data) {
		printf("%-24s: Callback : %s%s%s%s%s\n", p->desc,
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[0] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
		printf("%-24s: User     : %s%s%s%s%s\n", "",
		       (p->data[1] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[1] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[1] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[1] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[1] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
		printf("%-24s: Operator : %s%s%s%s%s\n", "",
		       (p->data[2] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[2] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[2] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[2] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[2] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
		printf("%-24s: Admin    : %s%s%s%s%s\n", "",
		       (p->data[3] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[3] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[3] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[3] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[3] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
		printf("%-24s: OEM      : %s%s%s%s%s\n", "",
		       (p->data[4] & 1<<IPMI_SESSION_AUTHTYPE_NONE) ? "NONE " : "",
		       (p->data[4] & 1<<IPMI_SESSION_AUTHTYPE_MD2) ? "MD2 " : "",
		       (p->data[4] & 1<<IPMI_SESSION_AUTHTYPE_MD5) ? "MD5 " : "",
		       (p->data[4] & 1<<IPMI_SESSION_AUTHTYPE_PASSWORD) ? "PASSWORD " : "",
		       (p->data[4] & 1<<IPMI_SESSION_AUTHTYPE_OEM) ? "OEM " : "");
	}

	p = get_lan_param(intf, chan, IPMI_LANP_IP_ADDR_SRC);
	if (!p)
		return -1;
	if (p->data) {
		printf("%-24s: ", p->desc);
		p->data[0] &= 0xf;
		switch (p->data[0]) {
		case 0:
			printf("Unspecified\n");
			break;
		case 1:
			printf("Static Address\n");
			break;
		case 2:
			printf("DHCP Address\n");
			break;
		case 3:
			printf("BIOS Assigned Address\n");
			break;
		default:
			printf("Other\n");
			break;
		}
	}

	p = get_lan_param(intf, chan, IPMI_LANP_IP_ADDR);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %d.%d.%d.%d\n", p->desc,
		       p->data[0], p->data[1], p->data[2], p->data[3]);

	p = get_lan_param(intf, chan, IPMI_LANP_SUBNET_MASK);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %d.%d.%d.%d\n", p->desc,
		       p->data[0], p->data[1], p->data[2], p->data[3]);

	p = get_lan_param(intf, chan, IPMI_LANP_MAC_ADDR);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %s\n", p->desc, mac2str(p->data));

	p = get_lan_param(intf, chan, IPMI_LANP_SNMP_STRING);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %s\n", p->desc, p->data);

	p = get_lan_param(intf, chan, IPMI_LANP_IP_HEADER);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: TTL=0x%02x Flags=0x%02x Precedence=0x%02x TOS=0x%02x\n",
		       p->desc, p->data[0], p->data[1] & 0xe0, p->data[2] & 0xe0, p->data[2] & 0x1e);

	p = get_lan_param(intf, chan, IPMI_LANP_BMC_ARP);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: ARP Responses %sabled, Gratuitous ARP %sabled\n", p->desc,
		       (p->data[0] & 2) ? "En" : "Dis", (p->data[0] & 1) ? "En" : "Dis");

	p = get_lan_param(intf, chan, IPMI_LANP_GRAT_ARP);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %.1f seconds\n", p->desc, (float)((p->data[0] + 1) / 2));

	p = get_lan_param(intf, chan, IPMI_LANP_DEF_GATEWAY_IP);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %d.%d.%d.%d\n", p->desc,
		       p->data[0], p->data[1], p->data[2], p->data[3]);

	p = get_lan_param(intf, chan, IPMI_LANP_DEF_GATEWAY_MAC);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %s\n", p->desc, mac2str(p->data));

	p = get_lan_param(intf, chan, IPMI_LANP_BAK_GATEWAY_IP);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %d.%d.%d.%d\n", p->desc,
		       p->data[0], p->data[1], p->data[2], p->data[3]);

	p = get_lan_param(intf, chan, IPMI_LANP_BAK_GATEWAY_MAC);
	if (!p)
		return -1;
	if (p->data)
		printf("%-24s: %s\n", p->desc, mac2str(p->data));

	p = get_lan_param(intf, chan, IPMI_LANP_VLAN_ID);
	if (p && p->data) {
		int id = ((p->data[1] & 0x0f) << 8) + p->data[0];
		if (p->data[1] & 0x80)
			printf("%-24s: %d\n", p->desc, id);
		else
			printf("%-24s: Disabled\n", p->desc);
	}

	p = get_lan_param(intf, chan, IPMI_LANP_VLAN_PRIORITY);
	if (p && p->data)
		printf("%-24s: %d\n", p->desc, p->data[0] & 0x07);

	/* Determine supported Cipher Suites -- Requires two calls */
	p = get_lan_param(intf, chan, IPMI_LANP_RMCP_CIPHER_SUPPORT);
	if (!p)
		return -1;
	else if (p->data)
	{
		unsigned char cipher_suite_count = p->data[0];
		p = get_lan_param(intf, chan, IPMI_LANP_RMCP_CIPHERS);
		if (!p)
			return -1;

		printf("%-24s: ", p->desc);

		/* Now we're dangerous.  There are only 15 fixed cipher
		   suite IDs, but the spec allows for 16 in the return data.*/
		if (p->data && p->data_len <= 17)
		{
			unsigned int i;
			for (i = 0; (i < 16) && (i < cipher_suite_count); ++i)
			{
				printf("%s%d",
				       (i > 0? ",": ""),
				       p->data[i + 1]);
			}
			printf("\n");
		}
		else
		{
			printf("None\n");
		}
	}

	/* RMCP+ Messaging Cipher Suite Privilege Levels */
	/* These are the privilege levels for the 15 fixed cipher suites */
	p = get_lan_param(intf, chan, IPMI_LANP_RMCP_PRIV_LEVELS);
	if (!p)
		return -1;
	if (p->data && 9 == p->data_len)
	{
		printf("%-24s: %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", p->desc,
		       priv_level_to_char(p->data[1] & 0x0F),
		       priv_level_to_char(p->data[1] >> 4),
		       priv_level_to_char(p->data[2] & 0x0F),
		       priv_level_to_char(p->data[2] >> 4),
		       priv_level_to_char(p->data[3] & 0x0F),
		       priv_level_to_char(p->data[3] >> 4),
		       priv_level_to_char(p->data[4] & 0x0F),
		       priv_level_to_char(p->data[4] >> 4),
		       priv_level_to_char(p->data[5] & 0x0F),
		       priv_level_to_char(p->data[5] >> 4),
		       priv_level_to_char(p->data[6] & 0x0F),
		       priv_level_to_char(p->data[6] >> 4),
		       priv_level_to_char(p->data[7] & 0x0F),
		       priv_level_to_char(p->data[7] >> 4),
		       priv_level_to_char(p->data[8] & 0x0F));

		/* Now print a legend */
		printf("%-24s: %s\n", "", "    X=Cipher Suite Unused");
		printf("%-24s: %s\n", "", "    c=CALLBACK");
		printf("%-24s: %s\n", "", "    u=USER");
		printf("%-24s: %s\n", "", "    o=OPERATOR");
		printf("%-24s: %s\n", "", "    a=ADMIN");
		printf("%-24s: %s\n", "", "    O=OEM");
	}
	else
		printf("%-24s: Not Available\n", p->desc);

	/* Bad Password Threshold */
	p = get_lan_param(intf, chan, IPMI_LANP_BAD_PASS_THRESH);
	if (!p)
		return -1;
	if (p->data && 6 == p->data_len) {
		int tmp;

		printf("%-24s: %d\n", p->desc, p->data[1]);
		printf("%-24s: %s\n", "Invalid password disable",
				p->data[0] & 1 ? "yes" : "no" );
		tmp = p->data[2] + (p->data[3] << 8);
		printf("%-24s: %d\n", "Attempt Count Reset Int.", tmp * 10);
		tmp = p->data[4] + (p->data[5] << 8);
		printf("%-24s: %d\n", "User Lockout Interval", tmp * 10);
	} else {
		printf("%-24s: Not Available\n", p->desc);
	}

	return 0;
}