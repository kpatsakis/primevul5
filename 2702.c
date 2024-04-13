PQresetPoll(PGconn *conn)
{
	if (conn)
	{
		PostgresPollingStatusType status = PQconnectPoll(conn);

		if (status == PGRES_POLLING_OK)
		{
			/*
			 * Notify event procs of successful reset.  We treat an event proc
			 * failure as disabling the connection ... good idea?
			 */
			int			i;

			for (i = 0; i < conn->nEvents; i++)
			{
				PGEventConnReset evt;

				evt.conn = conn;
				if (!conn->events[i].proc(PGEVT_CONNRESET, &evt,
										  conn->events[i].passThrough))
				{
					conn->status = CONNECTION_BAD;
					appendPQExpBuffer(&conn->errorMessage,
									  libpq_gettext("PGEventProc \"%s\" failed during PGEVT_CONNRESET event\n"),
									  conn->events[i].name);
					return PGRES_POLLING_FAILED;
				}
			}
		}

		return status;
	}

	return PGRES_POLLING_FAILED;
}