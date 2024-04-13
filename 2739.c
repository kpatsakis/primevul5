internal_ping(PGconn *conn)
{
	/* Say "no attempt" if we never got to PQconnectPoll */
	if (!conn || !conn->options_valid)
		return PQPING_NO_ATTEMPT;

	/* Attempt to complete the connection */
	if (conn->status != CONNECTION_BAD)
		(void) connectDBComplete(conn);

	/* Definitely OK if we succeeded */
	if (conn->status != CONNECTION_BAD)
		return PQPING_OK;

	/*
	 * Here begins the interesting part of "ping": determine the cause of the
	 * failure in sufficient detail to decide what to return.  We do not want
	 * to report that the server is not up just because we didn't have a valid
	 * password, for example.  In fact, any sort of authentication request
	 * implies the server is up.  (We need this check since the libpq side of
	 * things might have pulled the plug on the connection before getting an
	 * error as such from the postmaster.)
	 */
	if (conn->auth_req_received)
		return PQPING_OK;

	/*
	 * If we failed to get any ERROR response from the postmaster, report
	 * PQPING_NO_RESPONSE.  This result could be somewhat misleading for a
	 * pre-7.4 server, since it won't send back a SQLSTATE, but those are long
	 * out of support.  Another corner case where the server could return a
	 * failure without a SQLSTATE is fork failure, but PQPING_NO_RESPONSE
	 * isn't totally unreasonable for that anyway.  We expect that every other
	 * failure case in a modern server will produce a report with a SQLSTATE.
	 *
	 * NOTE: whenever we get around to making libpq generate SQLSTATEs for
	 * client-side errors, we should either not store those into
	 * last_sqlstate, or add an extra flag so we can tell client-side errors
	 * apart from server-side ones.
	 */
	if (strlen(conn->last_sqlstate) != 5)
		return PQPING_NO_RESPONSE;

	/*
	 * Report PQPING_REJECT if server says it's not accepting connections. (We
	 * distinguish this case mainly for the convenience of pg_ctl.)
	 */
	if (strcmp(conn->last_sqlstate, ERRCODE_CANNOT_CONNECT_NOW) == 0)
		return PQPING_REJECT;

	/*
	 * Any other SQLSTATE can be taken to indicate that the server is up.
	 * Presumably it didn't like our username, password, or database name; or
	 * perhaps it had some transient failure, but that should not be taken as
	 * meaning "it's down".
	 */
	return PQPING_OK;
}