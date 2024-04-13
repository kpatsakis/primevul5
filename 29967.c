NTSTATUS smbXcli_negprot(struct smbXcli_conn *conn,
			 uint32_t timeout_msec,
			 enum protocol_types min_protocol,
			 enum protocol_types max_protocol)
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
	req = smbXcli_negprot_send(frame, ev, conn, timeout_msec,
				   min_protocol, max_protocol);
	if (req == NULL) {
		goto fail;
	}
	ok = tevent_req_poll_ntstatus(req, ev, &status);
	if (!ok) {
		goto fail;
	}
	status = smbXcli_negprot_recv(req);
 fail:
	TALLOC_FREE(frame);
	return status;
}
