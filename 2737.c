conninfo_uri_parse_params(char *params,
						  PQconninfoOption *connOptions,
						  PQExpBuffer errorMessage)
{
	while (*params)
	{
		char	   *keyword = params;
		char	   *value = NULL;
		char	   *p = params;
		bool		malloced = false;
		int			oldmsglen;

		/*
		 * Scan the params string for '=' and '&', marking the end of keyword
		 * and value respectively.
		 */
		for (;;)
		{
			if (*p == '=')
			{
				/* Was there '=' already? */
				if (value != NULL)
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("extra key/value separator \"=\" in URI query parameter: \"%s\"\n"),
									  keyword);
					return false;
				}
				/* Cut off keyword, advance to value */
				*p++ = '\0';
				value = p;
			}
			else if (*p == '&' || *p == '\0')
			{
				/*
				 * If not at the end, cut off value and advance; leave p
				 * pointing to start of the next parameter, if any.
				 */
				if (*p != '\0')
					*p++ = '\0';
				/* Was there '=' at all? */
				if (value == NULL)
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("missing key/value separator \"=\" in URI query parameter: \"%s\"\n"),
									  keyword);
					return false;
				}
				/* Got keyword and value, go process them. */
				break;
			}
			else
				++p;			/* Advance over all other bytes. */
		}

		keyword = conninfo_uri_decode(keyword, errorMessage);
		if (keyword == NULL)
		{
			/* conninfo_uri_decode already set an error message */
			return false;
		}
		value = conninfo_uri_decode(value, errorMessage);
		if (value == NULL)
		{
			/* conninfo_uri_decode already set an error message */
			free(keyword);
			return false;
		}
		malloced = true;

		/*
		 * Special keyword handling for improved JDBC compatibility.
		 */
		if (strcmp(keyword, "ssl") == 0 &&
			strcmp(value, "true") == 0)
		{
			free(keyword);
			free(value);
			malloced = false;

			keyword = "sslmode";
			value = "require";
		}

		/*
		 * Store the value if the corresponding option exists; ignore
		 * otherwise.  At this point both keyword and value are not
		 * URI-encoded.
		 */
		oldmsglen = errorMessage->len;
		if (!conninfo_storeval(connOptions, keyword, value,
							   errorMessage, true, false))
		{
			/* Insert generic message if conninfo_storeval didn't give one. */
			if (errorMessage->len == oldmsglen)
				appendPQExpBuffer(errorMessage,
								  libpq_gettext("invalid URI query parameter: \"%s\"\n"),
								  keyword);
			/* And fail. */
			if (malloced)
			{
				free(keyword);
				free(value);
			}
			return false;
		}

		if (malloced)
		{
			free(keyword);
			free(value);
		}

		/* Proceed to next key=value pair, if any */
		params = p;
	}

	return true;
}