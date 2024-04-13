sendTerminateConn(PGconn *conn)
{
	/*
	 * Note that the protocol doesn't allow us to send Terminate messages
	 * during the startup phase.
	 */
	if (conn->sock != PGINVALID_SOCKET && conn->status == CONNECTION_OK)
	{
		/*
		 * Try to send "close connection" message to backend. Ignore any
		 * error.
		 */
		pqPutMsgStart('X', conn);
		pqPutMsgEnd(conn);
		(void) pqFlush(conn);
	}
}