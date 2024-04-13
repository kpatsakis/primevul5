struct tevent_req *smb1cli_req_send(TALLOC_CTX *mem_ctx,
				    struct tevent_context *ev,
				    struct smbXcli_conn *conn,
				    uint8_t smb_command,
				    uint8_t additional_flags,
				    uint8_t clear_flags,
				    uint16_t additional_flags2,
				    uint16_t clear_flags2,
				    uint32_t timeout_msec,
				    uint32_t pid,
				    struct smbXcli_tcon *tcon,
				    struct smbXcli_session *session,
				    uint8_t wct, uint16_t *vwv,
				    uint32_t num_bytes,
				    const uint8_t *bytes)
{
	struct tevent_req *req;
	struct iovec iov;
	NTSTATUS status;

	iov.iov_base = discard_const_p(void, bytes);
	iov.iov_len = num_bytes;

	req = smb1cli_req_create(mem_ctx, ev, conn, smb_command,
				 additional_flags, clear_flags,
				 additional_flags2, clear_flags2,
				 timeout_msec,
				 pid, tcon, session,
				 wct, vwv, 1, &iov);
	if (req == NULL) {
		return NULL;
	}
	if (!tevent_req_is_in_progress(req)) {
		return tevent_req_post(req, ev);
	}
	status = smb1cli_req_chain_submit(&req, 1);
	if (tevent_req_nterror(req, status)) {
		return tevent_req_post(req, ev);
	}
	return req;
}
