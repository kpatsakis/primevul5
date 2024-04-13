release_conn_addrinfo(PGconn *conn)
{
	if (conn->addrlist)
	{
		pg_freeaddrinfo_all(conn->addrlist_family, conn->addrlist);
		conn->addrlist = NULL;
		conn->addr_cur = NULL;	/* for safety */
	}
}