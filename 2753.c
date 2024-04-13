PQsocket(const PGconn *conn)
{
	if (!conn)
		return -1;
	return (conn->sock != PGINVALID_SOCKET) ? conn->sock : -1;
}