ldapServiceLookup(const char *purl, PQconninfoOption *options,
				  PQExpBuffer errorMessage)
{
	int			port = LDAP_DEF_PORT,
				scope,
				rc,
				size,
				state,
				oldstate,
				i;
#ifndef WIN32
	int			msgid;
#endif
	bool		found_keyword;
	char	   *url,
			   *hostname,
			   *portstr,
			   *endptr,
			   *dn,
			   *scopestr,
			   *filter,
			   *result,
			   *p,
			   *p1 = NULL,
			   *optname = NULL,
			   *optval = NULL;
	char	   *attrs[2] = {NULL, NULL};
	LDAP	   *ld = NULL;
	LDAPMessage *res,
			   *entry;
	struct berval **values;
	LDAP_TIMEVAL time = {PGLDAP_TIMEOUT, 0};

	if ((url = strdup(purl)) == NULL)
	{
		appendPQExpBufferStr(errorMessage, libpq_gettext("out of memory\n"));
		return 3;
	}

	/*
	 * Parse URL components, check for correctness.  Basically, url has '\0'
	 * placed at component boundaries and variables are pointed at each
	 * component.
	 */

	if (pg_strncasecmp(url, LDAP_URL, strlen(LDAP_URL)) != 0)
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": scheme must be ldap://\n"), purl);
		free(url);
		return 3;
	}

	/* hostname */
	hostname = url + strlen(LDAP_URL);
	if (*hostname == '/')		/* no hostname? */
		hostname = DefaultHost; /* the default */

	/* dn, "distinguished name" */
	p = strchr(url + strlen(LDAP_URL), '/');
	if (p == NULL || *(p + 1) == '\0' || *(p + 1) == '?')
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": missing distinguished name\n"),
						  purl);
		free(url);
		return 3;
	}
	*p = '\0';					/* terminate hostname */
	dn = p + 1;

	/* attribute */
	if ((p = strchr(dn, '?')) == NULL || *(p + 1) == '\0' || *(p + 1) == '?')
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": must have exactly one attribute\n"),
						  purl);
		free(url);
		return 3;
	}
	*p = '\0';
	attrs[0] = p + 1;

	/* scope */
	if ((p = strchr(attrs[0], '?')) == NULL || *(p + 1) == '\0' || *(p + 1) == '?')
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": must have search scope (base/one/sub)\n"),
						  purl);
		free(url);
		return 3;
	}
	*p = '\0';
	scopestr = p + 1;

	/* filter */
	if ((p = strchr(scopestr, '?')) == NULL || *(p + 1) == '\0' || *(p + 1) == '?')
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": no filter\n"),
						  purl);
		free(url);
		return 3;
	}
	*p = '\0';
	filter = p + 1;
	if ((p = strchr(filter, '?')) != NULL)
		*p = '\0';

	/* port number? */
	if ((p1 = strchr(hostname, ':')) != NULL)
	{
		long		lport;

		*p1 = '\0';
		portstr = p1 + 1;
		errno = 0;
		lport = strtol(portstr, &endptr, 10);
		if (*portstr == '\0' || *endptr != '\0' || errno || lport < 0 || lport > 65535)
		{
			appendPQExpBuffer(errorMessage,
							  libpq_gettext("invalid LDAP URL \"%s\": invalid port number\n"),
							  purl);
			free(url);
			return 3;
		}
		port = (int) lport;
	}

	/* Allow only one attribute */
	if (strchr(attrs[0], ',') != NULL)
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": must have exactly one attribute\n"),
						  purl);
		free(url);
		return 3;
	}

	/* set scope */
	if (pg_strcasecmp(scopestr, "base") == 0)
		scope = LDAP_SCOPE_BASE;
	else if (pg_strcasecmp(scopestr, "one") == 0)
		scope = LDAP_SCOPE_ONELEVEL;
	else if (pg_strcasecmp(scopestr, "sub") == 0)
		scope = LDAP_SCOPE_SUBTREE;
	else
	{
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("invalid LDAP URL \"%s\": must have search scope (base/one/sub)\n"),
						  purl);
		free(url);
		return 3;
	}

	/* initialize LDAP structure */
	if ((ld = ldap_init(hostname, port)) == NULL)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("could not create LDAP structure\n"));
		free(url);
		return 3;
	}

	/*
	 * Perform an explicit anonymous bind.
	 *
	 * LDAP does not require that an anonymous bind is performed explicitly,
	 * but we want to distinguish between the case where LDAP bind does not
	 * succeed within PGLDAP_TIMEOUT seconds (return 2 to continue parsing the
	 * service control file) and the case where querying the LDAP server fails
	 * (return 1 to end parsing).
	 *
	 * Unfortunately there is no way of setting a timeout that works for both
	 * Windows and OpenLDAP.
	 */
#ifdef WIN32
	/* the nonstandard ldap_connect function performs an anonymous bind */
	if (ldap_connect(ld, &time) != LDAP_SUCCESS)
	{
		/* error or timeout in ldap_connect */
		free(url);
		ldap_unbind(ld);
		return 2;
	}
#else							/* !WIN32 */
	/* in OpenLDAP, use the LDAP_OPT_NETWORK_TIMEOUT option */
	if (ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &time) != LDAP_SUCCESS)
	{
		free(url);
		ldap_unbind(ld);
		return 3;
	}

	/* anonymous bind */
	if ((msgid = ldap_simple_bind(ld, NULL, NULL)) == -1)
	{
		/* error or network timeout */
		free(url);
		ldap_unbind(ld);
		return 2;
	}

	/* wait some time for the connection to succeed */
	res = NULL;
	if ((rc = ldap_result(ld, msgid, LDAP_MSG_ALL, &time, &res)) == -1 ||
		res == NULL)
	{
		/* error or timeout */
		if (res != NULL)
			ldap_msgfree(res);
		free(url);
		ldap_unbind(ld);
		return 2;
	}
	ldap_msgfree(res);

	/* reset timeout */
	time.tv_sec = -1;
	if (ldap_set_option(ld, LDAP_OPT_NETWORK_TIMEOUT, &time) != LDAP_SUCCESS)
	{
		free(url);
		ldap_unbind(ld);
		return 3;
	}
#endif							/* WIN32 */

	/* search */
	res = NULL;
	if ((rc = ldap_search_st(ld, dn, scope, filter, attrs, 0, &time, &res))
		!= LDAP_SUCCESS)
	{
		if (res != NULL)
			ldap_msgfree(res);
		appendPQExpBuffer(errorMessage,
						  libpq_gettext("lookup on LDAP server failed: %s\n"),
						  ldap_err2string(rc));
		ldap_unbind(ld);
		free(url);
		return 1;
	}

	/* complain if there was not exactly one result */
	if ((rc = ldap_count_entries(ld, res)) != 1)
	{
		appendPQExpBufferStr(errorMessage,
							 rc ? libpq_gettext("more than one entry found on LDAP lookup\n")
							 : libpq_gettext("no entry found on LDAP lookup\n"));
		ldap_msgfree(res);
		ldap_unbind(ld);
		free(url);
		return 1;
	}

	/* get entry */
	if ((entry = ldap_first_entry(ld, res)) == NULL)
	{
		/* should never happen */
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("no entry found on LDAP lookup\n"));
		ldap_msgfree(res);
		ldap_unbind(ld);
		free(url);
		return 1;
	}

	/* get values */
	if ((values = ldap_get_values_len(ld, entry, attrs[0])) == NULL)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("attribute has no values on LDAP lookup\n"));
		ldap_msgfree(res);
		ldap_unbind(ld);
		free(url);
		return 1;
	}

	ldap_msgfree(res);
	free(url);

	if (values[0] == NULL)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("attribute has no values on LDAP lookup\n"));
		ldap_value_free_len(values);
		ldap_unbind(ld);
		return 1;
	}

	/* concatenate values into a single string with newline terminators */
	size = 1;					/* for the trailing null */
	for (i = 0; values[i] != NULL; i++)
		size += values[i]->bv_len + 1;
	if ((result = malloc(size)) == NULL)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("out of memory\n"));
		ldap_value_free_len(values);
		ldap_unbind(ld);
		return 3;
	}
	p = result;
	for (i = 0; values[i] != NULL; i++)
	{
		memcpy(p, values[i]->bv_val, values[i]->bv_len);
		p += values[i]->bv_len;
		*(p++) = '\n';
	}
	*p = '\0';

	ldap_value_free_len(values);
	ldap_unbind(ld);

	/* parse result string */
	oldstate = state = 0;
	for (p = result; *p != '\0'; ++p)
	{
		switch (state)
		{
			case 0:				/* between entries */
				if (!ld_is_sp_tab(*p) && !ld_is_nl_cr(*p))
				{
					optname = p;
					state = 1;
				}
				break;
			case 1:				/* in option name */
				if (ld_is_sp_tab(*p))
				{
					*p = '\0';
					state = 2;
				}
				else if (ld_is_nl_cr(*p))
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("missing \"=\" after \"%s\" in connection info string\n"),
									  optname);
					free(result);
					return 3;
				}
				else if (*p == '=')
				{
					*p = '\0';
					state = 3;
				}
				break;
			case 2:				/* after option name */
				if (*p == '=')
				{
					state = 3;
				}
				else if (!ld_is_sp_tab(*p))
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("missing \"=\" after \"%s\" in connection info string\n"),
									  optname);
					free(result);
					return 3;
				}
				break;
			case 3:				/* before option value */
				if (*p == '\'')
				{
					optval = p + 1;
					p1 = p + 1;
					state = 5;
				}
				else if (ld_is_nl_cr(*p))
				{
					optval = optname + strlen(optname); /* empty */
					state = 0;
				}
				else if (!ld_is_sp_tab(*p))
				{
					optval = p;
					state = 4;
				}
				break;
			case 4:				/* in unquoted option value */
				if (ld_is_sp_tab(*p) || ld_is_nl_cr(*p))
				{
					*p = '\0';
					state = 0;
				}
				break;
			case 5:				/* in quoted option value */
				if (*p == '\'')
				{
					*p1 = '\0';
					state = 0;
				}
				else if (*p == '\\')
					state = 6;
				else
					*(p1++) = *p;
				break;
			case 6:				/* in quoted option value after escape */
				*(p1++) = *p;
				state = 5;
				break;
		}

		if (state == 0 && oldstate != 0)
		{
			found_keyword = false;
			for (i = 0; options[i].keyword; i++)
			{
				if (strcmp(options[i].keyword, optname) == 0)
				{
					if (options[i].val == NULL)
					{
						options[i].val = strdup(optval);
						if (!options[i].val)
						{
							appendPQExpBufferStr(errorMessage,
												 libpq_gettext("out of memory\n"));
							free(result);
							return 3;
						}
					}
					found_keyword = true;
					break;
				}
			}
			if (!found_keyword)
			{
				appendPQExpBuffer(errorMessage,
								  libpq_gettext("invalid connection option \"%s\"\n"),
								  optname);
				free(result);
				return 1;
			}
			optname = NULL;
			optval = NULL;
		}
		oldstate = state;
	}

	free(result);

	if (state == 5 || state == 6)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("unterminated quoted string in connection info string\n"));
		return 3;
	}

	return 0;
}