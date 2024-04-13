PQhost(const PGconn *conn)
{
	if (!conn)
		return NULL;

	if (conn->connhost != NULL)
	{
		/*
		 * Return the verbatim host value provided by user, or hostaddr in its
		 * lack.
		 */
		if (conn->connhost[conn->whichhost].host != NULL &&
			conn->connhost[conn->whichhost].host[0] != '\0')
			return conn->connhost[conn->whichhost].host;
		else if (conn->connhost[conn->whichhost].hostaddr != NULL &&
				 conn->connhost[conn->whichhost].hostaddr[0] != '\0')
			return conn->connhost[conn->whichhost].hostaddr;
	}

	return "";
}