emitHostIdentityInfo(PGconn *conn, const char *host_addr)
{
#ifdef HAVE_UNIX_SOCKETS
	if (IS_AF_UNIX(conn->raddr.addr.ss_family))
	{
		char		service[NI_MAXHOST];

		pg_getnameinfo_all(&conn->raddr.addr, conn->raddr.salen,
						   NULL, 0,
						   service, sizeof(service),
						   NI_NUMERICSERV);
		appendPQExpBuffer(&conn->errorMessage,
						  libpq_gettext("connection to server on socket \"%s\" failed: "),
						  service);
	}
	else
#endif							/* HAVE_UNIX_SOCKETS */
	{
		const char *displayed_host;
		const char *displayed_port;

		/* To which host and port were we actually connecting? */
		if (conn->connhost[conn->whichhost].type == CHT_HOST_ADDRESS)
			displayed_host = conn->connhost[conn->whichhost].hostaddr;
		else
			displayed_host = conn->connhost[conn->whichhost].host;
		displayed_port = conn->connhost[conn->whichhost].port;
		if (displayed_port == NULL || displayed_port[0] == '\0')
			displayed_port = DEF_PGPORT_STR;

		/*
		 * If the user did not supply an IP address using 'hostaddr', and
		 * 'host' was missing or does not match our lookup, display the
		 * looked-up IP address.
		 */
		if (conn->connhost[conn->whichhost].type != CHT_HOST_ADDRESS &&
			host_addr[0] &&
			strcmp(displayed_host, host_addr) != 0)
			appendPQExpBuffer(&conn->errorMessage,
							  libpq_gettext("connection to server at \"%s\" (%s), port %s failed: "),
							  displayed_host, host_addr,
							  displayed_port);
		else
			appendPQExpBuffer(&conn->errorMessage,
							  libpq_gettext("connection to server at \"%s\", port %s failed: "),
							  displayed_host,
							  displayed_port);
	}
}