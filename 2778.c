PQuser(const PGconn *conn)
{
	if (!conn)
		return NULL;
	return conn->pguser;
}