parse_sockaddr(struct sockaddr *sa, int family, const char *str)
{
	struct in_addr		 ina;
	struct in6_addr		 in6a;
	struct sockaddr_in	*sin;
	struct sockaddr_in6	*sin6;
	char			*cp, *str2;
	const char		*errstr;

	switch (family) {
	case PF_UNSPEC:
		if (parse_sockaddr(sa, PF_INET, str) == 0)
			return (0);
		return parse_sockaddr(sa, PF_INET6, str);

	case PF_INET:
		if (inet_pton(PF_INET, str, &ina) != 1)
			return (-1);

		sin = (struct sockaddr_in *)sa;
		memset(sin, 0, sizeof *sin);
		sin->sin_len = sizeof(struct sockaddr_in);
		sin->sin_family = PF_INET;
		sin->sin_addr.s_addr = ina.s_addr;
		return (0);

	case PF_INET6:
		if (strncasecmp("ipv6:", str, 5) == 0)
			str += 5;
		cp = strchr(str, SCOPE_DELIMITER);
		if (cp) {
			str2 = strdup(str);
			if (str2 == NULL)
				return (-1);
			str2[cp - str] = '\0';
			if (inet_pton(PF_INET6, str2, &in6a) != 1) {
				free(str2);
				return (-1);
			}
			cp++;
			free(str2);
		} else if (inet_pton(PF_INET6, str, &in6a) != 1)
			return (-1);

		sin6 = (struct sockaddr_in6 *)sa;
		memset(sin6, 0, sizeof *sin6);
		sin6->sin6_len = sizeof(struct sockaddr_in6);
		sin6->sin6_family = PF_INET6;
		sin6->sin6_addr = in6a;

		if (cp == NULL)
			return (0);

		if (IN6_IS_ADDR_LINKLOCAL(&in6a) ||
		    IN6_IS_ADDR_MC_LINKLOCAL(&in6a) ||
		    IN6_IS_ADDR_MC_INTFACELOCAL(&in6a))
			if ((sin6->sin6_scope_id = if_nametoindex(cp)))
				return (0);

		sin6->sin6_scope_id = strtonum(cp, 0, UINT32_MAX, &errstr);
		if (errstr)
			return (-1);
		return (0);

	default:
		break;
	}

	return (-1);
}