fillPGconn(PGconn *conn, PQconninfoOption *connOptions)
{
	const internalPQconninfoOption *option;

	for (option = PQconninfoOptions; option->keyword; option++)
	{
		if (option->connofs >= 0)
		{
			const char *tmp = conninfo_getval(connOptions, option->keyword);

			if (tmp)
			{
				char	  **connmember = (char **) ((char *) conn + option->connofs);

				if (*connmember)
					free(*connmember);
				*connmember = strdup(tmp);
				if (*connmember == NULL)
				{
					appendPQExpBufferStr(&conn->errorMessage,
										 libpq_gettext("out of memory\n"));
					return false;
				}
			}
		}
	}

	return true;
}