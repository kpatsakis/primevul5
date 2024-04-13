static NTSTATUS smb1cli_conn_dispatch_incoming(struct smbXcli_conn *conn,
					       TALLOC_CTX *tmp_mem,
					       uint8_t *inbuf)
{
	struct tevent_req *req;
	struct smbXcli_req_state *state;
	NTSTATUS status;
	size_t num_pending;
	size_t i;
	uint8_t cmd;
	uint16_t mid;
	bool oplock_break;
	uint8_t *inhdr = inbuf + NBT_HDR_SIZE;
	size_t len = smb_len_tcp(inbuf);
	struct iovec *iov = NULL;
	int num_iov = 0;
	struct tevent_req **chain = NULL;
	size_t num_chained = 0;
	size_t num_responses = 0;

	if (conn->smb1.read_braw_req != NULL) {
		req = conn->smb1.read_braw_req;
		conn->smb1.read_braw_req = NULL;
		state = tevent_req_data(req, struct smbXcli_req_state);

		smbXcli_req_unset_pending(req);

		if (state->smb1.recv_iov == NULL) {
			/*
			 * For requests with more than
			 * one response, we have to readd the
			 * recv_iov array.
			 */
			state->smb1.recv_iov = talloc_zero_array(state,
								 struct iovec,
								 3);
			if (tevent_req_nomem(state->smb1.recv_iov, req)) {
				return NT_STATUS_OK;
			}
		}

		state->smb1.recv_iov[0].iov_base = (void *)(inhdr);
		state->smb1.recv_iov[0].iov_len = len;
		ZERO_STRUCT(state->smb1.recv_iov[1]);
		ZERO_STRUCT(state->smb1.recv_iov[2]);

		state->smb1.recv_cmd = SMBreadBraw;
		state->smb1.recv_status = NT_STATUS_OK;
		state->inbuf = talloc_move(state->smb1.recv_iov, &inbuf);

		tevent_req_done(req);
		return NT_STATUS_OK;
	}

	if ((IVAL(inhdr, 0) != SMB_MAGIC) /* 0xFF"SMB" */
	    && (SVAL(inhdr, 0) != 0x45ff)) /* 0xFF"E" */ {
		DEBUG(10, ("Got non-SMB PDU\n"));
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	/*
	 * If we supported multiple encrytion contexts
	 * here we'd look up based on tid.
	 */
	if (common_encryption_on(conn->smb1.trans_enc)
	    && (CVAL(inbuf, 0) == 0)) {
		uint16_t enc_ctx_num;

		status = get_enc_ctx_num(inbuf, &enc_ctx_num);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("get_enc_ctx_num returned %s\n",
				   nt_errstr(status)));
			return status;
		}

		if (enc_ctx_num != conn->smb1.trans_enc->enc_ctx_num) {
			DEBUG(10, ("wrong enc_ctx %d, expected %d\n",
				   enc_ctx_num,
				   conn->smb1.trans_enc->enc_ctx_num));
			return NT_STATUS_INVALID_HANDLE;
		}

		status = common_decrypt_buffer(conn->smb1.trans_enc,
					       (char *)inbuf);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("common_decrypt_buffer returned %s\n",
				   nt_errstr(status)));
			return status;
		}
		inhdr = inbuf + NBT_HDR_SIZE;
		len = smb_len_nbt(inbuf);
	}

	mid = SVAL(inhdr, HDR_MID);
	num_pending = talloc_array_length(conn->pending);

	for (i=0; i<num_pending; i++) {
		if (mid == smb1cli_req_mid(conn->pending[i])) {
			break;
		}
	}
	if (i == num_pending) {
		/* Dump unexpected reply */
		return NT_STATUS_RETRY;
	}

	oplock_break = false;

	if (mid == 0xffff) {
		/*
		 * Paranoia checks that this is really an oplock break request.
		 */
		oplock_break = (len == 51); /* hdr + 8 words */
		oplock_break &= ((CVAL(inhdr, HDR_FLG) & FLAG_REPLY) == 0);
		oplock_break &= (CVAL(inhdr, HDR_COM) == SMBlockingX);
		oplock_break &= (SVAL(inhdr, HDR_VWV+VWV(6)) == 0);
		oplock_break &= (SVAL(inhdr, HDR_VWV+VWV(7)) == 0);

		if (!oplock_break) {
			/* Dump unexpected reply */
			return NT_STATUS_RETRY;
		}
	}

	req = conn->pending[i];
	state = tevent_req_data(req, struct smbXcli_req_state);

	if (!oplock_break /* oplock breaks are not signed */
	    && !smb_signing_check_pdu(conn->smb1.signing,
				      inhdr, len, state->smb1.seqnum+1)) {
		DEBUG(10, ("cli_check_sign_mac failed\n"));
		return NT_STATUS_ACCESS_DENIED;
	}

	status = smb1cli_inbuf_parse_chain(inbuf, tmp_mem,
					   &iov, &num_iov);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10,("smb1cli_inbuf_parse_chain - %s\n",
			  nt_errstr(status)));
		return status;
	}

	cmd = CVAL(inhdr, HDR_COM);
	status = smb1cli_pull_raw_error(inhdr);

	if (NT_STATUS_EQUAL(status, NT_STATUS_NETWORK_SESSION_EXPIRED) &&
	    (state->session != NULL) && state->session->disconnect_expired)
	{
		/*
		 * this should be a short term hack
		 * until the upper layers have implemented
		 * re-authentication.
		 */
		return status;
	}

	if (state->smb1.chained_requests == NULL) {
		if (num_iov != 3) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}

		smbXcli_req_unset_pending(req);

		if (state->smb1.recv_iov == NULL) {
			/*
			 * For requests with more than
			 * one response, we have to readd the
			 * recv_iov array.
			 */
			state->smb1.recv_iov = talloc_zero_array(state,
								 struct iovec,
								 3);
			if (tevent_req_nomem(state->smb1.recv_iov, req)) {
				return NT_STATUS_OK;
			}
		}

		state->smb1.recv_cmd = cmd;
		state->smb1.recv_status = status;
		state->inbuf = talloc_move(state->smb1.recv_iov, &inbuf);

		state->smb1.recv_iov[0] = iov[0];
		state->smb1.recv_iov[1] = iov[1];
		state->smb1.recv_iov[2] = iov[2];

		if (talloc_array_length(conn->pending) == 0) {
			tevent_req_done(req);
			return NT_STATUS_OK;
		}

		tevent_req_defer_callback(req, state->ev);
		tevent_req_done(req);
		return NT_STATUS_RETRY;
	}

	chain = talloc_move(tmp_mem, &state->smb1.chained_requests);
	num_chained = talloc_array_length(chain);
	num_responses = (num_iov - 1)/2;

	if (num_responses > num_chained) {
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	for (i=0; i<num_chained; i++) {
		size_t iov_idx = 1 + (i*2);
		struct iovec *cur = &iov[iov_idx];
		uint8_t *inbuf_ref;

		req = chain[i];
		state = tevent_req_data(req, struct smbXcli_req_state);

		smbXcli_req_unset_pending(req);

		/*
		 * as we finish multiple requests here
		 * we need to defer the callbacks as
		 * they could destroy our current stack state.
		 */
		tevent_req_defer_callback(req, state->ev);

		if (i >= num_responses) {
			tevent_req_nterror(req, NT_STATUS_REQUEST_ABORTED);
			continue;
		}

		if (state->smb1.recv_iov == NULL) {
			/*
			 * For requests with more than
			 * one response, we have to readd the
			 * recv_iov array.
			 */
			state->smb1.recv_iov = talloc_zero_array(state,
								 struct iovec,
								 3);
			if (tevent_req_nomem(state->smb1.recv_iov, req)) {
				continue;
			}
		}

		state->smb1.recv_cmd = cmd;

		if (i == (num_responses - 1)) {
			/*
			 * The last request in the chain gets the status
			 */
			state->smb1.recv_status = status;
		} else {
			cmd = CVAL(cur[0].iov_base, 0);
			state->smb1.recv_status = NT_STATUS_OK;
		}

		state->inbuf = inbuf;

		/*
		 * Note: here we use talloc_reference() in a way
		 *       that does not expose it to the caller.
		 */
		inbuf_ref = talloc_reference(state->smb1.recv_iov, inbuf);
		if (tevent_req_nomem(inbuf_ref, req)) {
			continue;
		}

		/* copy the related buffers */
		state->smb1.recv_iov[0] = iov[0];
		state->smb1.recv_iov[1] = cur[0];
		state->smb1.recv_iov[2] = cur[1];

		tevent_req_done(req);
	}

	return NT_STATUS_RETRY;
}
