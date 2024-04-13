setKeepalivesIdle(PGconn *conn)
{
	int			idle;

	if (conn->keepalives_idle == NULL)
		return 1;

	if (!parse_int_param(conn->keepalives_idle, &idle, conn,
						 "keepalives_idle"))
		return 0;
	if (idle < 0)
		idle = 0;

#ifdef PG_TCP_KEEPALIVE_IDLE
	if (setsockopt(conn->sock, IPPROTO_TCP, PG_TCP_KEEPALIVE_IDLE,
				   (char *) &idle, sizeof(idle)) < 0)
	{
		char		sebuf[PG_STRERROR_R_BUFLEN];

		appendPQExpBuffer(&conn->errorMessage,
						  libpq_gettext("%s(%s) failed: %s\n"),
						  "setsockopt",
						  PG_TCP_KEEPALIVE_IDLE_STR,
						  SOCK_STRERROR(SOCK_ERRNO, sebuf, sizeof(sebuf)));
		return 0;
	}
#endif

	return 1;
}