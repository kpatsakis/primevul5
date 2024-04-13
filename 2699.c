PQpass(const PGconn *conn)
{
	char	   *password = NULL;

	if (!conn)
		return NULL;
	if (conn->connhost != NULL)
		password = conn->connhost[conn->whichhost].password;
	if (password == NULL)
		password = conn->pgpass;
	/* Historically we've returned "" not NULL for no password specified */
	if (password == NULL)
		password = "";
	return password;
}