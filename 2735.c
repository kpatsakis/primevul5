closePGconn(PGconn *conn)
{
	/*
	 * If possible, send Terminate message to close the connection politely.
	 */
	sendTerminateConn(conn);

	/*
	 * Must reset the blocking status so a possible reconnect will work.
	 *
	 * Don't call PQsetnonblocking() because it will fail if it's unable to
	 * flush the connection.
	 */
	conn->nonblocking = false;

	/*
	 * Close the connection, reset all transient state, flush I/O buffers.
	 * Note that this includes clearing conn->errorMessage; we're no longer
	 * interested in any failures associated with the old connection, and we
	 * want a clean slate for any new connection attempt.
	 */
	pqDropConnection(conn, true);
	conn->status = CONNECTION_BAD;	/* Well, not really _bad_ - just absent */
	conn->asyncStatus = PGASYNC_IDLE;
	conn->xactStatus = PQTRANS_IDLE;
	conn->pipelineStatus = PQ_PIPELINE_OFF;
	pqClearAsyncResult(conn);	/* deallocate result */
	resetPQExpBuffer(&conn->errorMessage);
	release_conn_addrinfo(conn);

	/* Reset all state obtained from server, too */
	pqDropServerData(conn);
}