conninfo_uri_decode(const char *str, PQExpBuffer errorMessage)
{
	char	   *buf;
	char	   *p;
	const char *q = str;

	buf = malloc(strlen(str) + 1);
	if (buf == NULL)
	{
		appendPQExpBufferStr(errorMessage, libpq_gettext("out of memory\n"));
		return NULL;
	}
	p = buf;

	for (;;)
	{
		if (*q != '%')
		{
			/* copy and check for NUL terminator */
			if (!(*(p++) = *(q++)))
				break;
		}
		else
		{
			int			hi;
			int			lo;
			int			c;

			++q;				/* skip the percent sign itself */

			/*
			 * Possible EOL will be caught by the first call to
			 * get_hexdigit(), so we never dereference an invalid q pointer.
			 */
			if (!(get_hexdigit(*q++, &hi) && get_hexdigit(*q++, &lo)))
			{
				appendPQExpBuffer(errorMessage,
								  libpq_gettext("invalid percent-encoded token: \"%s\"\n"),
								  str);
				free(buf);
				return NULL;
			}

			c = (hi << 4) | lo;
			if (c == 0)
			{
				appendPQExpBuffer(errorMessage,
								  libpq_gettext("forbidden value %%00 in percent-encoded value: \"%s\"\n"),
								  str);
				free(buf);
				return NULL;
			}
			*(p++) = c;
		}
	}

	return buf;
}