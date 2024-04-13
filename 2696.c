makeEmptyPGconn(void)
{
	PGconn	   *conn;

#ifdef WIN32

	/*
	 * Make sure socket support is up and running in this process.
	 *
	 * Note: the Windows documentation says that we should eventually do a
	 * matching WSACleanup() call, but experience suggests that that is at
	 * least as likely to cause problems as fix them.  So we don't.
	 */
	static bool wsastartup_done = false;

	if (!wsastartup_done)
	{
		WSADATA		wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return NULL;
		wsastartup_done = true;
	}

	/* Forget any earlier error */
	WSASetLastError(0);
#endif							/* WIN32 */

	conn = (PGconn *) malloc(sizeof(PGconn));
	if (conn == NULL)
		return conn;

	/* Zero all pointers and booleans */
	MemSet(conn, 0, sizeof(PGconn));

	/* install default notice hooks */
	conn->noticeHooks.noticeRec = defaultNoticeReceiver;
	conn->noticeHooks.noticeProc = defaultNoticeProcessor;

	conn->status = CONNECTION_BAD;
	conn->asyncStatus = PGASYNC_IDLE;
	conn->pipelineStatus = PQ_PIPELINE_OFF;
	conn->xactStatus = PQTRANS_IDLE;
	conn->options_valid = false;
	conn->nonblocking = false;
	conn->client_encoding = PG_SQL_ASCII;
	conn->std_strings = false;	/* unless server says differently */
	conn->default_transaction_read_only = PG_BOOL_UNKNOWN;
	conn->in_hot_standby = PG_BOOL_UNKNOWN;
	conn->verbosity = PQERRORS_DEFAULT;
	conn->show_context = PQSHOW_CONTEXT_ERRORS;
	conn->sock = PGINVALID_SOCKET;
	conn->Pfdebug = NULL;

	/*
	 * We try to send at least 8K at a time, which is the usual size of pipe
	 * buffers on Unix systems.  That way, when we are sending a large amount
	 * of data, we avoid incurring extra kernel context swaps for partial
	 * bufferloads.  The output buffer is initially made 16K in size, and we
	 * try to dump it after accumulating 8K.
	 *
	 * With the same goal of minimizing context swaps, the input buffer will
	 * be enlarged anytime it has less than 8K free, so we initially allocate
	 * twice that.
	 */
	conn->inBufSize = 16 * 1024;
	conn->inBuffer = (char *) malloc(conn->inBufSize);
	conn->outBufSize = 16 * 1024;
	conn->outBuffer = (char *) malloc(conn->outBufSize);
	conn->rowBufLen = 32;
	conn->rowBuf = (PGdataValue *) malloc(conn->rowBufLen * sizeof(PGdataValue));
	initPQExpBuffer(&conn->errorMessage);
	initPQExpBuffer(&conn->workBuffer);

	if (conn->inBuffer == NULL ||
		conn->outBuffer == NULL ||
		conn->rowBuf == NULL ||
		PQExpBufferBroken(&conn->errorMessage) ||
		PQExpBufferBroken(&conn->workBuffer))
	{
		/* out of memory already :-( */
		freePGconn(conn);
		conn = NULL;
	}

	return conn;
}