parseServiceFile(const char *serviceFile,
				 const char *service,
				 PQconninfoOption *options,
				 PQExpBuffer errorMessage,
				 bool *group_found)
{
	int			result = 0,
				linenr = 0,
				i;
	FILE	   *f;
	char	   *line;
	char		buf[1024];

	*group_found = false;

	f = fopen(serviceFile, "r");
	if (f == NULL)
	{
		appendPQExpBuffer(errorMessage, libpq_gettext("service file \"%s\" not found\n"),
						  serviceFile);
		return 1;
	}

	while ((line = fgets(buf, sizeof(buf), f)) != NULL)
	{
		int			len;

		linenr++;

		if (strlen(line) >= sizeof(buf) - 1)
		{
			appendPQExpBuffer(errorMessage,
							  libpq_gettext("line %d too long in service file \"%s\"\n"),
							  linenr,
							  serviceFile);
			result = 2;
			goto exit;
		}

		/* ignore whitespace at end of line, especially the newline */
		len = strlen(line);
		while (len > 0 && isspace((unsigned char) line[len - 1]))
			line[--len] = '\0';

		/* ignore leading whitespace too */
		while (*line && isspace((unsigned char) line[0]))
			line++;

		/* ignore comments and empty lines */
		if (line[0] == '\0' || line[0] == '#')
			continue;

		/* Check for right groupname */
		if (line[0] == '[')
		{
			if (*group_found)
			{
				/* end of desired group reached; return success */
				goto exit;
			}

			if (strncmp(line + 1, service, strlen(service)) == 0 &&
				line[strlen(service) + 1] == ']')
				*group_found = true;
			else
				*group_found = false;
		}
		else
		{
			if (*group_found)
			{
				/*
				 * Finally, we are in the right group and can parse the line
				 */
				char	   *key,
						   *val;
				bool		found_keyword;

#ifdef USE_LDAP
				if (strncmp(line, "ldap", 4) == 0)
				{
					int			rc = ldapServiceLookup(line, options, errorMessage);

					/* if rc = 2, go on reading for fallback */
					switch (rc)
					{
						case 0:
							goto exit;
						case 1:
						case 3:
							result = 3;
							goto exit;
						case 2:
							continue;
					}
				}
#endif

				key = line;
				val = strchr(line, '=');
				if (val == NULL)
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("syntax error in service file \"%s\", line %d\n"),
									  serviceFile,
									  linenr);
					result = 3;
					goto exit;
				}
				*val++ = '\0';

				if (strcmp(key, "service") == 0)
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("nested service specifications not supported in service file \"%s\", line %d\n"),
									  serviceFile,
									  linenr);
					result = 3;
					goto exit;
				}

				/*
				 * Set the parameter --- but don't override any previous
				 * explicit setting.
				 */
				found_keyword = false;
				for (i = 0; options[i].keyword; i++)
				{
					if (strcmp(options[i].keyword, key) == 0)
					{
						if (options[i].val == NULL)
							options[i].val = strdup(val);
						if (!options[i].val)
						{
							appendPQExpBufferStr(errorMessage,
												 libpq_gettext("out of memory\n"));
							result = 3;
							goto exit;
						}
						found_keyword = true;
						break;
					}
				}

				if (!found_keyword)
				{
					appendPQExpBuffer(errorMessage,
									  libpq_gettext("syntax error in service file \"%s\", line %d\n"),
									  serviceFile,
									  linenr);
					result = 3;
					goto exit;
				}
			}
		}
	}

exit:
	fclose(f);

	return result;
}