connectDBStart(PGconn *conn)
{
	if (!conn)
		return 0;

	if (!conn->options_valid)
		goto connect_errReturn;

	/*
	 * Check for bad linking to backend-internal versions of src/common
	 * functions (see comments in link-canary.c for the reason we need this).
	 * Nobody but developers should see this message, so we don't bother
	 * translating it.
	 */
	if (!pg_link_canary_is_frontend())
	{
		appendPQExpBufferStr(&conn->errorMessage,
							 "libpq is incorrectly linked to backend functions\n");
		goto connect_errReturn;
	}

	/* Ensure our buffers are empty */
	conn->inStart = conn->inCursor = conn->inEnd = 0;
	conn->outCount = 0;

	/*
	 * Set up to try to connect to the first host.  (Setting whichhost = -1 is
	 * a bit of a cheat, but PQconnectPoll will advance it to 0 before
	 * anything else looks at it.)
	 */
	conn->whichhost = -1;
	conn->try_next_addr = false;
	conn->try_next_host = true;
	conn->status = CONNECTION_NEEDED;

	/* Also reset the target_server_type state if needed */
	if (conn->target_server_type == SERVER_TYPE_PREFER_STANDBY_PASS2)
		conn->target_server_type = SERVER_TYPE_PREFER_STANDBY;

	/*
	 * The code for processing CONNECTION_NEEDED state is in PQconnectPoll(),
	 * so that it can easily be re-executed if needed again during the
	 * asynchronous startup process.  However, we must run it once here,
	 * because callers expect a success return from this routine to mean that
	 * we are in PGRES_POLLING_WRITING connection state.
	 */
	if (PQconnectPoll(conn) == PGRES_POLLING_WRITING)
		return 1;

connect_errReturn:

	/*
	 * If we managed to open a socket, close it immediately rather than
	 * waiting till PQfinish.  (The application cannot have gotten the socket
	 * from PQsocket yet, so this doesn't risk breaking anything.)
	 */
	pqDropConnection(conn, true);
	conn->status = CONNECTION_BAD;
	return 0;
}