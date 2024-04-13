connectFailureMessage(PGconn *conn, int errorno)
{
	char		sebuf[PG_STRERROR_R_BUFLEN];

	appendPQExpBuffer(&conn->errorMessage,
					  "%s\n",
					  SOCK_STRERROR(errorno, sebuf, sizeof(sebuf)));

#ifdef HAVE_UNIX_SOCKETS
	if (IS_AF_UNIX(conn->raddr.addr.ss_family))
		appendPQExpBufferStr(&conn->errorMessage,
							 libpq_gettext("\tIs the server running locally and accepting connections on that socket?\n"));
	else
#endif
		appendPQExpBufferStr(&conn->errorMessage,
							 libpq_gettext("\tIs the server running on that host and accepting TCP/IP connections?\n"));
}