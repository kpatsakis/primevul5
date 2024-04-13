passwordFromFile(const char *hostname, const char *port, const char *dbname,
				 const char *username, const char *pgpassfile)
{
	FILE	   *fp;
	struct stat stat_buf;
	PQExpBufferData buf;

	if (dbname == NULL || dbname[0] == '\0')
		return NULL;

	if (username == NULL || username[0] == '\0')
		return NULL;

	/* 'localhost' matches pghost of '' or the default socket directory */
	if (hostname == NULL || hostname[0] == '\0')
		hostname = DefaultHost;
	else if (is_unixsock_path(hostname))

		/*
		 * We should probably use canonicalize_path(), but then we have to
		 * bring path.c into libpq, and it doesn't seem worth it.
		 */
		if (strcmp(hostname, DEFAULT_PGSOCKET_DIR) == 0)
			hostname = DefaultHost;

	if (port == NULL || port[0] == '\0')
		port = DEF_PGPORT_STR;

	/* If password file cannot be opened, ignore it. */
	if (stat(pgpassfile, &stat_buf) != 0)
		return NULL;

#ifndef WIN32
	if (!S_ISREG(stat_buf.st_mode))
	{
		fprintf(stderr,
				libpq_gettext("WARNING: password file \"%s\" is not a plain file\n"),
				pgpassfile);
		return NULL;
	}

	/* If password file is insecure, alert the user and ignore it. */
	if (stat_buf.st_mode & (S_IRWXG | S_IRWXO))
	{
		fprintf(stderr,
				libpq_gettext("WARNING: password file \"%s\" has group or world access; permissions should be u=rw (0600) or less\n"),
				pgpassfile);
		return NULL;
	}
#else

	/*
	 * On Win32, the directory is protected, so we don't have to check the
	 * file.
	 */
#endif

	fp = fopen(pgpassfile, "r");
	if (fp == NULL)
		return NULL;

	/* Use an expansible buffer to accommodate any reasonable line length */
	initPQExpBuffer(&buf);

	while (!feof(fp) && !ferror(fp))
	{
		/* Make sure there's a reasonable amount of room in the buffer */
		if (!enlargePQExpBuffer(&buf, 128))
			break;

		/* Read some data, appending it to what we already have */
		if (fgets(buf.data + buf.len, buf.maxlen - buf.len, fp) == NULL)
			break;
		buf.len += strlen(buf.data + buf.len);

		/* If we don't yet have a whole line, loop around to read more */
		if (!(buf.len > 0 && buf.data[buf.len - 1] == '\n') && !feof(fp))
			continue;

		/* ignore comments */
		if (buf.data[0] != '#')
		{
			char	   *t = buf.data;
			int			len;

			/* strip trailing newline and carriage return */
			len = pg_strip_crlf(t);

			if (len > 0 &&
				(t = pwdfMatchesString(t, hostname)) != NULL &&
				(t = pwdfMatchesString(t, port)) != NULL &&
				(t = pwdfMatchesString(t, dbname)) != NULL &&
				(t = pwdfMatchesString(t, username)) != NULL)
			{
				/* Found a match. */
				char	   *ret,
						   *p1,
						   *p2;

				ret = strdup(t);

				fclose(fp);
				explicit_bzero(buf.data, buf.maxlen);
				termPQExpBuffer(&buf);

				if (!ret)
				{
					/* Out of memory. XXX: an error message would be nice. */
					return NULL;
				}

				/* De-escape password. */
				for (p1 = p2 = ret; *p1 != ':' && *p1 != '\0'; ++p1, ++p2)
				{
					if (*p1 == '\\' && p1[1] != '\0')
						++p1;
					*p2 = *p1;
				}
				*p2 = '\0';

				return ret;
			}
		}

		/* No match, reset buffer to prepare for next line. */
		buf.len = 0;
	}

	fclose(fp);
	explicit_bzero(buf.data, buf.maxlen);
	termPQExpBuffer(&buf);
	return NULL;
}