PQport(const PGconn *conn)
{
	if (!conn)
		return NULL;

	if (conn->connhost != NULL)
		return conn->connhost[conn->whichhost].port;

	return "";
}