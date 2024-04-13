PQconninfo(PGconn *conn)
{
	PQExpBufferData errorBuf;
	PQconninfoOption *connOptions;

	if (conn == NULL)
		return NULL;

	/*
	 * We don't actually report any errors here, but callees want a buffer,
	 * and we prefer not to trash the conn's errorMessage.
	 */
	initPQExpBuffer(&errorBuf);
	if (PQExpBufferDataBroken(errorBuf))
		return NULL;			/* out of memory already :-( */

	connOptions = conninfo_init(&errorBuf);

	if (connOptions != NULL)
	{
		const internalPQconninfoOption *option;

		for (option = PQconninfoOptions; option->keyword; option++)
		{
			char	  **connmember;

			if (option->connofs < 0)
				continue;

			connmember = (char **) ((char *) conn + option->connofs);

			if (*connmember)
				conninfo_storeval(connOptions, option->keyword, *connmember,
								  &errorBuf, true, false);
		}
	}

	termPQExpBuffer(&errorBuf);

	return connOptions;
}