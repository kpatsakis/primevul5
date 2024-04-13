PQhostaddr(const PGconn *conn)
{
	if (!conn)
		return NULL;

	/* Return the parsed IP address */
	if (conn->connhost != NULL && conn->connip != NULL)
		return conn->connip;

	return "";
}