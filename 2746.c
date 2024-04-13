pqDropServerData(PGconn *conn)
{
	PGnotify   *notify;
	pgParameterStatus *pstatus;

	/* Forget pending notifies */
	notify = conn->notifyHead;
	while (notify != NULL)
	{
		PGnotify   *prev = notify;

		notify = notify->next;
		free(prev);
	}
	conn->notifyHead = conn->notifyTail = NULL;

	pqFreeCommandQueue(conn->cmd_queue_head);
	conn->cmd_queue_head = conn->cmd_queue_tail = NULL;

	pqFreeCommandQueue(conn->cmd_queue_recycle);
	conn->cmd_queue_recycle = NULL;

	/* Reset ParameterStatus data, as well as variables deduced from it */
	pstatus = conn->pstatus;
	while (pstatus != NULL)
	{
		pgParameterStatus *prev = pstatus;

		pstatus = pstatus->next;
		free(prev);
	}
	conn->pstatus = NULL;
	conn->client_encoding = PG_SQL_ASCII;
	conn->std_strings = false;
	conn->default_transaction_read_only = PG_BOOL_UNKNOWN;
	conn->in_hot_standby = PG_BOOL_UNKNOWN;
	conn->sversion = 0;

	/* Drop large-object lookup data */
	if (conn->lobjfuncs)
		free(conn->lobjfuncs);
	conn->lobjfuncs = NULL;

	/* Reset assorted other per-connection state */
	conn->last_sqlstate[0] = '\0';
	conn->auth_req_received = false;
	conn->password_needed = false;
	conn->write_failed = false;
	if (conn->write_err_msg)
		free(conn->write_err_msg);
	conn->write_err_msg = NULL;
	conn->be_pid = 0;
	conn->be_key = 0;
}