static NTSTATUS smb1cli_req_writev_submit(struct tevent_req *req,
					  struct smbXcli_req_state *state,
					  struct iovec *iov, int iov_count)
{
	struct tevent_req *subreq;
	NTSTATUS status;
	uint8_t cmd;
	uint16_t mid;
	ssize_t nbtlen;

	if (!smbXcli_conn_is_connected(state->conn)) {
		return NT_STATUS_CONNECTION_DISCONNECTED;
	}

	if (state->conn->protocol > PROTOCOL_NT1) {
		return NT_STATUS_REVISION_MISMATCH;
	}

	if (iov_count < 4) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[0].iov_len != NBT_HDR_SIZE) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[1].iov_len != (MIN_SMB_SIZE-sizeof(uint16_t))) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[2].iov_len > (0xFF * sizeof(uint16_t))) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[3].iov_len != sizeof(uint16_t)) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}

	cmd = CVAL(iov[1].iov_base, HDR_COM);
	if (cmd == SMBreadBraw) {
		if (smbXcli_conn_has_async_calls(state->conn)) {
			return NT_STATUS_INVALID_PARAMETER_MIX;
		}
		state->conn->smb1.read_braw_req = req;
	}

	if (state->smb1.mid != 0) {
		mid = state->smb1.mid;
	} else {
		mid = smb1cli_alloc_mid(state->conn);
	}
	SSVAL(iov[1].iov_base, HDR_MID, mid);

	nbtlen = iov_buflen(&iov[1], iov_count-1);
	if ((nbtlen == -1) || (nbtlen > 0x1FFFF)) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}

	_smb_setlen_nbt(iov[0].iov_base, nbtlen);

	status = smb1cli_conn_signv(state->conn, iov, iov_count,
				    &state->smb1.seqnum,
				    state->smb1.one_way_seqnum);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/*
	 * If we supported multiple encrytion contexts
	 * here we'd look up based on tid.
	 */
	if (common_encryption_on(state->conn->smb1.trans_enc)) {
		char *buf, *enc_buf;

		buf = (char *)smbXcli_iov_concat(talloc_tos(), iov, iov_count);
		if (buf == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
		status = common_encrypt_buffer(state->conn->smb1.trans_enc,
					       (char *)buf, &enc_buf);
		TALLOC_FREE(buf);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(0, ("Error in encrypting client message: %s\n",
				  nt_errstr(status)));
			return status;
		}
		buf = (char *)talloc_memdup(state, enc_buf,
					    smb_len_nbt(enc_buf)+4);
		SAFE_FREE(enc_buf);
		if (buf == NULL) {
			return NT_STATUS_NO_MEMORY;
		}
		iov[0].iov_base = (void *)buf;
		iov[0].iov_len = talloc_get_size(buf);
		iov_count = 1;
	}

	if (state->conn->dispatch_incoming == NULL) {
		state->conn->dispatch_incoming = smb1cli_conn_dispatch_incoming;
	}

	if (!smbXcli_req_set_pending(req)) {
		return NT_STATUS_NO_MEMORY;
	}

	tevent_req_set_cancel_fn(req, smbXcli_req_cancel);

	subreq = writev_send(state, state->ev, state->conn->outgoing,
			     state->conn->sock_fd, false, iov, iov_count);
	if (subreq == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	tevent_req_set_callback(subreq, smb1cli_req_writev_done, req);
	state->write_req = subreq;

	return NT_STATUS_OK;
}
