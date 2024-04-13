setTCPUserTimeout(PGconn *conn)
{
	int			timeout;

	if (conn->pgtcp_user_timeout == NULL)
		return 1;

	if (!parse_int_param(conn->pgtcp_user_timeout, &timeout, conn,
						 "tcp_user_timeout"))
		return 0;

	if (timeout < 0)
		timeout = 0;

#ifdef TCP_USER_TIMEOUT
	if (setsockopt(conn->sock, IPPROTO_TCP, TCP_USER_TIMEOUT,
				   (char *) &timeout, sizeof(timeout)) < 0)
	{
		char		sebuf[256];

		appendPQExpBuffer(&conn->errorMessage,
						  libpq_gettext("%s(%s) failed: %s\n"),
						  "setsockopt",
						  "TCP_USER_TIMEOUT",
						  SOCK_STRERROR(SOCK_ERRNO, sebuf, sizeof(sebuf)));
		return 0;
	}
#endif

	return 1;
}