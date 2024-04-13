table_service_name(enum table_service s)
{
	switch (s) {
	case K_NONE:		return "NONE";
	case K_ALIAS:		return "ALIAS";
	case K_DOMAIN:		return "DOMAIN";
	case K_CREDENTIALS:	return "CREDENTIALS";
	case K_NETADDR:		return "NETADDR";
	case K_USERINFO:	return "USERINFO";
	case K_SOURCE:		return "SOURCE";
	case K_MAILADDR:	return "MAILADDR";
	case K_ADDRNAME:	return "ADDRNAME";
	case K_MAILADDRMAP:	return "MAILADDRMAP";
	case K_RELAYHOST:	return "RELAYHOST";
	case K_STRING:		return "STRING";
	case K_REGEX:		return "REGEX";
	}
	return "???";
}