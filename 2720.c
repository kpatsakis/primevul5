conninfo_add_defaults(PQconninfoOption *options, PQExpBuffer errorMessage)
{
	PQconninfoOption *option;
	char	   *tmp;

	/*
	 * If there's a service spec, use it to obtain any not-explicitly-given
	 * parameters.  Ignore error if no error message buffer is passed because
	 * there is no way to pass back the failure message.
	 */
	if (parseServiceInfo(options, errorMessage) != 0 && errorMessage)
		return false;

	/*
	 * Get the fallback resources for parameters not specified in the conninfo
	 * string nor the service.
	 */
	for (option = options; option->keyword != NULL; option++)
	{
		if (option->val != NULL)
			continue;			/* Value was in conninfo or service */

		/*
		 * Try to get the environment variable fallback
		 */
		if (option->envvar != NULL)
		{
			if ((tmp = getenv(option->envvar)) != NULL)
			{
				option->val = strdup(tmp);
				if (!option->val)
				{
					if (errorMessage)
						appendPQExpBufferStr(errorMessage,
											 libpq_gettext("out of memory\n"));
					return false;
				}
				continue;
			}
		}

		/*
		 * Interpret the deprecated PGREQUIRESSL environment variable.  Per
		 * tradition, translate values starting with "1" to sslmode=require,
		 * and ignore other values.  Given both PGREQUIRESSL=1 and PGSSLMODE,
		 * PGSSLMODE takes precedence; the opposite was true before v9.3.
		 */
		if (strcmp(option->keyword, "sslmode") == 0)
		{
			const char *requiresslenv = getenv("PGREQUIRESSL");

			if (requiresslenv != NULL && requiresslenv[0] == '1')
			{
				option->val = strdup("require");
				if (!option->val)
				{
					if (errorMessage)
						appendPQExpBufferStr(errorMessage,
											 libpq_gettext("out of memory\n"));
					return false;
				}
				continue;
			}
		}

		/*
		 * No environment variable specified or the variable isn't set - try
		 * compiled-in default
		 */
		if (option->compiled != NULL)
		{
			option->val = strdup(option->compiled);
			if (!option->val)
			{
				if (errorMessage)
					appendPQExpBufferStr(errorMessage,
										 libpq_gettext("out of memory\n"));
				return false;
			}
			continue;
		}

		/*
		 * Special handling for "user" option.  Note that if pg_fe_getauthname
		 * fails, we just leave the value as NULL; there's no need for this to
		 * be an error condition if the caller provides a user name.  The only
		 * reason we do this now at all is so that callers of PQconndefaults
		 * will see a correct default (barring error, of course).
		 */
		if (strcmp(option->keyword, "user") == 0)
		{
			option->val = pg_fe_getauthname(NULL);
			continue;
		}
	}

	return true;
}