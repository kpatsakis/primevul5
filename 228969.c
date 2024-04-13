ipmi_lan_set_auth(struct ipmi_intf * intf, uint8_t chan, char * level, char * types)
{
	uint8_t data[5];
	uint8_t authtype = 0;
	char * p;
	struct lan_param * lp;

	if (!level || !types)
		return -1;

	lp = get_lan_param(intf, chan, IPMI_LANP_AUTH_TYPE_ENABLE);
	if (!lp)
		return -1;
	if (!lp->data)
		return -1;

	lprintf(LOG_DEBUG, "%-24s: callback=0x%02x user=0x%02x operator=0x%02x admin=0x%02x oem=0x%02x",
		lp->desc, lp->data[0], lp->data[1], lp->data[2], lp->data[3], lp->data[4]);

	memset(data, 0, 5);
	memcpy(data, lp->data, 5);

	p = types;
	while (p) {
		if (strncasecmp(p, "none", 4) == 0)
			authtype |= 1 << IPMI_SESSION_AUTHTYPE_NONE;
		else if (strncasecmp(p, "md2", 3) == 0)
			authtype |= 1 << IPMI_SESSION_AUTHTYPE_MD2;
		else if (strncasecmp(p, "md5", 3) == 0)
			authtype |= 1 << IPMI_SESSION_AUTHTYPE_MD5;
		else if ((strncasecmp(p, "password", 8) == 0) ||
			 (strncasecmp(p, "key", 3) == 0))
			authtype |= 1 << IPMI_SESSION_AUTHTYPE_KEY;
		else if (strncasecmp(p, "oem", 3) == 0)
			authtype |= 1 << IPMI_SESSION_AUTHTYPE_OEM;
		else
			lprintf(LOG_WARNING, "Invalid authentication type: %s", p);
		p = strchr(p, ',');
		if (p)
			p++;
	}

	p = level;
	while (p) {
		if (strncasecmp(p, "callback", 8) == 0)
			data[0] = authtype;
		else if (strncasecmp(p, "user", 4) == 0)
			data[1] = authtype;
		else if (strncasecmp(p, "operator", 8) == 0)
			data[2] = authtype;
		else if (strncasecmp(p, "admin", 5) == 0)
			data[3] = authtype;
		else
			lprintf(LOG_WARNING, "Invalid authentication level: %s", p);
		p = strchr(p, ',');
		if (p)
			p++;
	}

	if (verbose > 1)
		printbuf(data, 5, "authtype data");

	return set_lan_param(intf, chan, IPMI_LANP_AUTH_TYPE_ENABLE, data, 5);
}