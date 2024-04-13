table_parse_lookup(enum table_service service, const char *key,
    const char *line, union lookup *lk)
{
	char	buffer[LINE_MAX], *p;
	size_t	len;

	len = strlen(line);

	switch (service) {
	case K_ALIAS:
		lk->expand = calloc(1, sizeof(*lk->expand));
		if (lk->expand == NULL)
			return (-1);
		if (!expand_line(lk->expand, line, 1)) {
			expand_free(lk->expand);
			return (-1);
		}
		return (1);

	case K_DOMAIN:
		if (strlcpy(lk->domain.name, line, sizeof(lk->domain.name))
		    >= sizeof(lk->domain.name))
			return (-1);
		return (1);

	case K_CREDENTIALS:

		/* credentials are stored as user:password */
		if (len < 3)
			return (-1);

		/* too big to fit in a smtp session line */
		if (len >= LINE_MAX)
			return (-1);

		p = strchr(line, ':');
		if (p == NULL) {
			if (strlcpy(lk->creds.username, key, sizeof (lk->creds.username))
			    >= sizeof (lk->creds.username))
				return (-1);
			if (strlcpy(lk->creds.password, line, sizeof(lk->creds.password))
			    >= sizeof(lk->creds.password))
				return (-1);
			return (1);
		}

		if (p == line || p == line + len - 1)
			return (-1);

		memmove(lk->creds.username, line, p - line);
		lk->creds.username[p - line] = '\0';

		if (strlcpy(lk->creds.password, p+1, sizeof(lk->creds.password))
		    >= sizeof(lk->creds.password))
			return (-1);

		return (1);

	case K_NETADDR:
		if (!text_to_netaddr(&lk->netaddr, line))
			return (-1);
		return (1);

	case K_USERINFO:
		if (!bsnprintf(buffer, sizeof(buffer), "%s:%s", key, line))
			return (-1);
		if (!text_to_userinfo(&lk->userinfo, buffer))
			return (-1);
 		return (1);

	case K_SOURCE:
		if (parse_sockaddr((struct sockaddr *)&lk->source.addr,
		    PF_UNSPEC, line) == -1)
			return (-1);
		return (1);

	case K_MAILADDR:
		if (!text_to_mailaddr(&lk->mailaddr, line))
			return (-1);
		return (1);

	case K_MAILADDRMAP:
		lk->maddrmap = calloc(1, sizeof(*lk->maddrmap));
		if (lk->maddrmap == NULL)
			return (-1);
		maddrmap_init(lk->maddrmap);
		if (!mailaddr_line(lk->maddrmap, line)) {
			maddrmap_free(lk->maddrmap);
			return (-1);
		}
		return (1);

	case K_ADDRNAME:
		if (parse_sockaddr((struct sockaddr *)&lk->addrname.addr,
		    PF_UNSPEC, key) == -1)
			return (-1);
		if (strlcpy(lk->addrname.name, line, sizeof(lk->addrname.name))
		    >= sizeof(lk->addrname.name))
			return (-1);
		return (1);

	case K_RELAYHOST:
		if (strlcpy(lk->relayhost, line, sizeof(lk->relayhost))
		    >= sizeof(lk->relayhost))
			return (-1);
		return (1);

	default:
		return (-1);
	}
}