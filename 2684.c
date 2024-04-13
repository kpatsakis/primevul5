pqDropConnection(PGconn *conn, bool flushInput)
{
	/* Drop any SSL state */
	pqsecure_close(conn);

	/* Close the socket itself */
	if (conn->sock != PGINVALID_SOCKET)
		closesocket(conn->sock);
	conn->sock = PGINVALID_SOCKET;

	/* Optionally discard any unread data */
	if (flushInput)
		conn->inStart = conn->inCursor = conn->inEnd = 0;

	/* Always discard any unsent data */
	conn->outCount = 0;

	/* Free authentication/encryption state */
#ifdef ENABLE_GSS
	{
		OM_uint32	min_s;

		if (conn->gcred != GSS_C_NO_CREDENTIAL)
		{
			gss_release_cred(&min_s, &conn->gcred);
			conn->gcred = GSS_C_NO_CREDENTIAL;
		}
		if (conn->gctx)
			gss_delete_sec_context(&min_s, &conn->gctx, GSS_C_NO_BUFFER);
		if (conn->gtarg_nam)
			gss_release_name(&min_s, &conn->gtarg_nam);
		if (conn->gss_SendBuffer)
		{
			free(conn->gss_SendBuffer);
			conn->gss_SendBuffer = NULL;
		}
		if (conn->gss_RecvBuffer)
		{
			free(conn->gss_RecvBuffer);
			conn->gss_RecvBuffer = NULL;
		}
		if (conn->gss_ResultBuffer)
		{
			free(conn->gss_ResultBuffer);
			conn->gss_ResultBuffer = NULL;
		}
		conn->gssenc = false;
	}
#endif
#ifdef ENABLE_SSPI
	if (conn->sspitarget)
	{
		free(conn->sspitarget);
		conn->sspitarget = NULL;
	}
	if (conn->sspicred)
	{
		FreeCredentialsHandle(conn->sspicred);
		free(conn->sspicred);
		conn->sspicred = NULL;
	}
	if (conn->sspictx)
	{
		DeleteSecurityContext(conn->sspictx);
		free(conn->sspictx);
		conn->sspictx = NULL;
	}
	conn->usesspi = 0;
#endif
	if (conn->sasl_state)
	{
		conn->sasl->free(conn->sasl_state);
		conn->sasl_state = NULL;
	}
}