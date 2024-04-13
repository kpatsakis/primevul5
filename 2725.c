useKeepalives(PGconn *conn)
{
	char	   *ep;
	int			val;

	if (conn->keepalives == NULL)
		return 1;
	val = strtol(conn->keepalives, &ep, 10);
	if (*ep)
		return -1;
	return val != 0 ? 1 : 0;
}