PQresetStart(PGconn *conn)
{
	if (conn)
	{
		closePGconn(conn);

		return connectDBStart(conn);
	}

	return 0;
}