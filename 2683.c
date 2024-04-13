setKeepalivesWin32(PGconn *conn)
{
	struct tcp_keepalive ka;
	DWORD		retsize;
	int			idle = 0;
	int			interval = 0;

	if (conn->keepalives_idle &&
		!parse_int_param(conn->keepalives_idle, &idle, conn,
						 "keepalives_idle"))
		return 0;
	if (idle <= 0)
		idle = 2 * 60 * 60;		/* 2 hours = default */

	if (conn->keepalives_interval &&
		!parse_int_param(conn->keepalives_interval, &interval, conn,
						 "keepalives_interval"))
		return 0;
	if (interval <= 0)
		interval = 1;			/* 1 second = default */

	ka.onoff = 1;
	ka.keepalivetime = idle * 1000;
	ka.keepaliveinterval = interval * 1000;

	if (WSAIoctl(conn->sock,
				 SIO_KEEPALIVE_VALS,
				 (LPVOID) &ka,
				 sizeof(ka),
				 NULL,
				 0,
				 &retsize,
				 NULL,
				 NULL)
		!= 0)
	{
		appendPQExpBuffer(&conn->errorMessage,
						  libpq_gettext("%s(%s) failed: error code %d\n"),
						  "WSAIoctl", "SIO_KEEPALIVE_VALS",
						  WSAGetLastError());
		return 0;
	}
	return 1;
}