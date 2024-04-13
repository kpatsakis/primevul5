conninfo_storeval(PQconninfoOption *connOptions,
				  const char *keyword, const char *value,
				  PQExpBuffer errorMessage, bool ignoreMissing,
				  bool uri_decode)
{
	PQconninfoOption *option;
	char	   *value_copy;

	/*
	 * For backwards compatibility, requiressl=1 gets translated to
	 * sslmode=require, and requiressl=0 gets translated to sslmode=prefer
	 * (which is the default for sslmode).
	 */
	if (strcmp(keyword, "requiressl") == 0)
	{
		keyword = "sslmode";
		if (value[0] == '1')
			value = "require";
		else
			value = "prefer";
	}

	option = conninfo_find(connOptions, keyword);
	if (option == NULL)
	{
		if (!ignoreMissing)
			appendPQExpBuffer(errorMessage,
							  libpq_gettext("invalid connection option \"%s\"\n"),
							  keyword);
		return NULL;
	}

	if (uri_decode)
	{
		value_copy = conninfo_uri_decode(value, errorMessage);
		if (value_copy == NULL)
			/* conninfo_uri_decode already set an error message */
			return NULL;
	}
	else
	{
		value_copy = strdup(value);
		if (value_copy == NULL)
		{
			appendPQExpBufferStr(errorMessage, libpq_gettext("out of memory\n"));
			return NULL;
		}
	}

	if (option->val)
		free(option->val);
	option->val = value_copy;

	return option;
}