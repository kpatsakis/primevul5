PQerrorMessage(const PGconn *conn)
{
	if (!conn)
		return libpq_gettext("connection pointer is NULL\n");

	/*
	 * The errorMessage buffer might be marked "broken" due to having
	 * previously failed to allocate enough memory for the message.  In that
	 * case, tell the application we ran out of memory.
	 */
	if (PQExpBufferBroken(&conn->errorMessage))
		return libpq_gettext("out of memory\n");

	return conn->errorMessage.data;
}