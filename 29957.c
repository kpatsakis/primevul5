NTSTATUS smbXcli_conn_samba_suicide(struct smbXcli_conn *conn,
				    uint8_t exitcode)
{
	TALLOC_CTX *frame = talloc_stackframe();
	struct tevent_context *ev;
	struct tevent_req *req;
	NTSTATUS status = NT_STATUS_NO_MEMORY;
	bool ok;

	if (smbXcli_conn_has_async_calls(conn)) {
		/*
		 * Can't use sync call while an async call is in flight
		 */
		status = NT_STATUS_INVALID_PARAMETER_MIX;
		goto fail;
	}
	ev = samba_tevent_context_init(frame);
	if (ev == NULL) {
		goto fail;
	}
	req = smbXcli_conn_samba_suicide_send(frame, ev, conn, exitcode);
	if (req == NULL) {
		goto fail;
	}
	ok = tevent_req_poll_ntstatus(req, ev, &status);
	if (!ok) {
		goto fail;
	}
	status = smbXcli_conn_samba_suicide_recv(req);
 fail:
	TALLOC_FREE(frame);
	return status;
}
