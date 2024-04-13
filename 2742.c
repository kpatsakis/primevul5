PQsetdbLogin(const char *pghost, const char *pgport, const char *pgoptions,
			 const char *pgtty, const char *dbName, const char *login,
			 const char *pwd)
{
	PGconn	   *conn;

	/*
	 * Allocate memory for the conn structure.  Note that we also expect this
	 * to initialize conn->errorMessage to empty.  All subsequent steps during
	 * connection initialization will only append to that buffer.
	 */
	conn = makeEmptyPGconn();
	if (conn == NULL)
		return NULL;

	/*
	 * If the dbName parameter contains what looks like a connection string,
	 * parse it into conn struct using connectOptions1.
	 */
	if (dbName && recognized_connection_string(dbName))
	{
		if (!connectOptions1(conn, dbName))
			return conn;
	}
	else
	{
		/*
		 * Old-style path: first, parse an empty conninfo string in order to
		 * set up the same defaults that PQconnectdb() would use.
		 */
		if (!connectOptions1(conn, ""))
			return conn;

		/* Insert dbName parameter value into struct */
		if (dbName && dbName[0] != '\0')
		{
			if (conn->dbName)
				free(conn->dbName);
			conn->dbName = strdup(dbName);
			if (!conn->dbName)
				goto oom_error;
		}
	}

	/*
	 * Insert remaining parameters into struct, overriding defaults (as well
	 * as any conflicting data from dbName taken as a conninfo).
	 */
	if (pghost && pghost[0] != '\0')
	{
		if (conn->pghost)
			free(conn->pghost);
		conn->pghost = strdup(pghost);
		if (!conn->pghost)
			goto oom_error;
	}

	if (pgport && pgport[0] != '\0')
	{
		if (conn->pgport)
			free(conn->pgport);
		conn->pgport = strdup(pgport);
		if (!conn->pgport)
			goto oom_error;
	}

	if (pgoptions && pgoptions[0] != '\0')
	{
		if (conn->pgoptions)
			free(conn->pgoptions);
		conn->pgoptions = strdup(pgoptions);
		if (!conn->pgoptions)
			goto oom_error;
	}

	if (login && login[0] != '\0')
	{
		if (conn->pguser)
			free(conn->pguser);
		conn->pguser = strdup(login);
		if (!conn->pguser)
			goto oom_error;
	}

	if (pwd && pwd[0] != '\0')
	{
		if (conn->pgpass)
			free(conn->pgpass);
		conn->pgpass = strdup(pwd);
		if (!conn->pgpass)
			goto oom_error;
	}

	/*
	 * Compute derived options
	 */
	if (!connectOptions2(conn))
		return conn;

	/*
	 * Connect to the database
	 */
	if (connectDBStart(conn))
		(void) connectDBComplete(conn);

	return conn;

oom_error:
	conn->status = CONNECTION_BAD;
	appendPQExpBufferStr(&conn->errorMessage,
						 libpq_gettext("out of memory\n"));
	return conn;
}