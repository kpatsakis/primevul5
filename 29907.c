struct tevent_req *smb1cli_req_create(TALLOC_CTX *mem_ctx,
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
				      int iov_count,
				      struct iovec *bytes_iov)
{
	struct tevent_req *req;
	struct smbXcli_req_state *state;
	uint8_t flags = 0;
	uint16_t flags2 = 0;
	uint16_t uid = 0;
	uint16_t tid = 0;
	ssize_t num_bytes;

	if (iov_count > MAX_SMB_IOV) {
		/*
		 * Should not happen :-)
		 */
		return NULL;
	}

	req = tevent_req_create(mem_ctx, &state,
				struct smbXcli_req_state);
	if (req == NULL) {
		return NULL;
	}
	state->ev = ev;
	state->conn = conn;
	state->session = session;
	state->tcon = tcon;

	if (session) {
		uid = session->smb1.session_id;
	}

	if (tcon) {
		tid = tcon->smb1.tcon_id;

		if (tcon->fs_attributes & FILE_CASE_SENSITIVE_SEARCH) {
			clear_flags |= FLAG_CASELESS_PATHNAMES;
		} else {
			/* Default setting, case insensitive. */
			additional_flags |= FLAG_CASELESS_PATHNAMES;
		}

		if (smbXcli_conn_dfs_supported(conn) &&
		    smbXcli_tcon_is_dfs_share(tcon))
		{
			additional_flags2 |= FLAGS2_DFS_PATHNAMES;
		}
	}

	state->smb1.recv_cmd = 0xFF;
	state->smb1.recv_status = NT_STATUS_INTERNAL_ERROR;
	state->smb1.recv_iov = talloc_zero_array(state, struct iovec, 3);
	if (state->smb1.recv_iov == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}

	smb1cli_req_flags(conn->protocol,
			  conn->smb1.capabilities,
			  smb_command,
			  additional_flags,
			  clear_flags,
			  &flags,
			  additional_flags2,
			  clear_flags2,
			  &flags2);

	SIVAL(state->smb1.hdr, 0,           SMB_MAGIC);
	SCVAL(state->smb1.hdr, HDR_COM,     smb_command);
	SIVAL(state->smb1.hdr, HDR_RCLS,    NT_STATUS_V(NT_STATUS_OK));
	SCVAL(state->smb1.hdr, HDR_FLG,     flags);
	SSVAL(state->smb1.hdr, HDR_FLG2,    flags2);
	SSVAL(state->smb1.hdr, HDR_PIDHIGH, pid >> 16);
	SSVAL(state->smb1.hdr, HDR_TID,     tid);
	SSVAL(state->smb1.hdr, HDR_PID,     pid);
	SSVAL(state->smb1.hdr, HDR_UID,     uid);
	SSVAL(state->smb1.hdr, HDR_MID,     0); /* this comes later */
	SCVAL(state->smb1.hdr, HDR_WCT,     wct);

	state->smb1.vwv = vwv;

	num_bytes = iov_buflen(bytes_iov, iov_count);
	if (num_bytes == -1) {
		/*
		 * I'd love to add a check for num_bytes<=UINT16_MAX here, but
		 * the smbclient->samba connections can lie and transfer more.
		 */
		TALLOC_FREE(req);
		return NULL;
	}

	SSVAL(state->smb1.bytecount_buf, 0, num_bytes);

	state->smb1.iov[0].iov_base = (void *)state->length_hdr;
	state->smb1.iov[0].iov_len  = sizeof(state->length_hdr);
	state->smb1.iov[1].iov_base = (void *)state->smb1.hdr;
	state->smb1.iov[1].iov_len  = sizeof(state->smb1.hdr);
	state->smb1.iov[2].iov_base = (void *)state->smb1.vwv;
	state->smb1.iov[2].iov_len  = wct * sizeof(uint16_t);
	state->smb1.iov[3].iov_base = (void *)state->smb1.bytecount_buf;
	state->smb1.iov[3].iov_len  = sizeof(uint16_t);

	if (iov_count != 0) {
		memcpy(&state->smb1.iov[4], bytes_iov,
		       iov_count * sizeof(*bytes_iov));
	}
	state->smb1.iov_count = iov_count + 4;

	if (timeout_msec > 0) {
		struct timeval endtime;

		endtime = timeval_current_ofs_msec(timeout_msec);
		if (!tevent_req_set_endtime(req, ev, endtime)) {
			return req;
		}
	}

	switch (smb_command) {
	case SMBtranss:
	case SMBtranss2:
	case SMBnttranss:
		state->one_way = true;
		break;
	case SMBntcancel:
		state->one_way = true;
		state->smb1.one_way_seqnum = true;
		break;
	case SMBlockingX:
		if ((wct == 8) &&
		    (CVAL(vwv+3, 0) == LOCKING_ANDX_OPLOCK_RELEASE)) {
			state->one_way = true;
		}
		break;
	}

	return req;
}
