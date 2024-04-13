PQdb(const PGconn *conn)
{
	if (!conn)
		return NULL;
	return conn->dbName;
}