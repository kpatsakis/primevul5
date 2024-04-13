struct tevent_req *smb2cli_req_send(TALLOC_CTX *mem_ctx,
				    struct tevent_context *ev,
				    struct smbXcli_conn *conn,
				    uint16_t cmd,
				    uint32_t additional_flags,
				    uint32_t clear_flags,
				    uint32_t timeout_msec,
				    struct smbXcli_tcon *tcon,
				    struct smbXcli_session *session,
				    const uint8_t *fixed,
				    uint16_t fixed_len,
				    const uint8_t *dyn,
				    uint32_t dyn_len,
				    uint32_t max_dyn_len)
{
	struct tevent_req *req;
	NTSTATUS status;

	req = smb2cli_req_create(mem_ctx, ev, conn, cmd,
				 additional_flags, clear_flags,
				 timeout_msec,
				 tcon, session,
				 fixed, fixed_len,
				 dyn, dyn_len,
				 max_dyn_len);
	if (req == NULL) {
		return NULL;
	}
	if (!tevent_req_is_in_progress(req)) {
		return tevent_req_post(req, ev);
	}
	status = smb2cli_req_compound_submit(&req, 1);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}
	return req;
}
