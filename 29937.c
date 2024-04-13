NTSTATUS smb2cli_req_compound_submit(struct tevent_req **reqs,
				     int num_reqs)
{
	struct smbXcli_req_state *state;
	struct tevent_req *subreq;
	struct iovec *iov;
	int i, num_iov, nbt_len;
	int tf_iov = -1;
	const DATA_BLOB *encryption_key = NULL;
	uint64_t encryption_session_id = 0;
	uint64_t nonce_high = UINT64_MAX;
	uint64_t nonce_low = UINT64_MAX;

	/*
	 * 1 for the nbt length, optional TRANSFORM
	 * per request: HDR, fixed, dyn, padding
	 * -1 because the last one does not need padding
	 */

	iov = talloc_array(reqs[0], struct iovec, 1 + 1 + 4*num_reqs - 1);
	if (iov == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	num_iov = 1;
	nbt_len = 0;

	/*
	 * the session of the first request that requires encryption
	 * specifies the encryption key.
	 */
	for (i=0; i<num_reqs; i++) {
		if (!tevent_req_is_in_progress(reqs[i])) {
			return NT_STATUS_INTERNAL_ERROR;
		}

		state = tevent_req_data(reqs[i], struct smbXcli_req_state);

		if (!smbXcli_conn_is_connected(state->conn)) {
			return NT_STATUS_CONNECTION_DISCONNECTED;
		}

		if ((state->conn->protocol != PROTOCOL_NONE) &&
		    (state->conn->protocol < PROTOCOL_SMB2_02)) {
			return NT_STATUS_REVISION_MISMATCH;
		}

		if (state->session == NULL) {
			continue;
		}

		if (!state->smb2.should_encrypt) {
			continue;
		}

		encryption_key = &state->session->smb2->encryption_key;
		if (encryption_key->length == 0) {
			return NT_STATUS_INVALID_PARAMETER_MIX;
		}

		encryption_session_id = state->session->smb2->session_id;

		state->session->smb2->nonce_low += 1;
		if (state->session->smb2->nonce_low == 0) {
			state->session->smb2->nonce_high += 1;
			state->session->smb2->nonce_low += 1;
		}

		/*
		 * CCM and GCM algorithms must never have their
		 * nonce wrap, or the security of the whole
		 * communication and the keys is destroyed.
		 * We must drop the connection once we have
		 * transfered too much data.
		 *
		 * NOTE: We assume nonces greater than 8 bytes.
		 */
		if (state->session->smb2->nonce_high >=
		    state->session->smb2->nonce_high_max)
		{
			return NT_STATUS_ENCRYPTION_FAILED;
		}

		nonce_high = state->session->smb2->nonce_high_random;
		nonce_high += state->session->smb2->nonce_high;
		nonce_low = state->session->smb2->nonce_low;

		tf_iov = num_iov;
		iov[num_iov].iov_base = state->smb2.transform;
		iov[num_iov].iov_len  = sizeof(state->smb2.transform);
		num_iov += 1;

		SBVAL(state->smb2.transform, SMB2_TF_PROTOCOL_ID, SMB2_TF_MAGIC);
		SBVAL(state->smb2.transform, SMB2_TF_NONCE,
		      nonce_low);
		SBVAL(state->smb2.transform, SMB2_TF_NONCE+8,
		      nonce_high);
		SBVAL(state->smb2.transform, SMB2_TF_SESSION_ID,
		      encryption_session_id);

		nbt_len += SMB2_TF_HDR_SIZE;
		break;
	}

	for (i=0; i<num_reqs; i++) {
		int hdr_iov;
		size_t reqlen;
		bool ret;
		uint16_t opcode;
		uint64_t avail;
		uint16_t charge;
		uint16_t credits;
		uint64_t mid;
		const DATA_BLOB *signing_key = NULL;

		if (!tevent_req_is_in_progress(reqs[i])) {
			return NT_STATUS_INTERNAL_ERROR;
		}

		state = tevent_req_data(reqs[i], struct smbXcli_req_state);

		if (!smbXcli_conn_is_connected(state->conn)) {
			return NT_STATUS_CONNECTION_DISCONNECTED;
		}

		if ((state->conn->protocol != PROTOCOL_NONE) &&
		    (state->conn->protocol < PROTOCOL_SMB2_02)) {
			return NT_STATUS_REVISION_MISMATCH;
		}

		opcode = SVAL(state->smb2.hdr, SMB2_HDR_OPCODE);
		if (opcode == SMB2_OP_CANCEL) {
			goto skip_credits;
		}

		avail = UINT64_MAX - state->conn->smb2.mid;
		if (avail < 1) {
			return NT_STATUS_CONNECTION_ABORTED;
		}

		if (state->conn->smb2.server.capabilities & SMB2_CAP_LARGE_MTU) {
			uint32_t max_dyn_len = 1;

			max_dyn_len = MAX(max_dyn_len, state->smb2.dyn_len);
			max_dyn_len = MAX(max_dyn_len, state->smb2.max_dyn_len);

			charge = (max_dyn_len - 1)/ 65536 + 1;
		} else {
			charge = 1;
		}

		charge = MAX(state->smb2.credit_charge, charge);

		avail = MIN(avail, state->conn->smb2.cur_credits);
		if (avail < charge) {
			return NT_STATUS_INTERNAL_ERROR;
		}

		credits = 0;
		if (state->conn->smb2.max_credits > state->conn->smb2.cur_credits) {
			credits = state->conn->smb2.max_credits -
				  state->conn->smb2.cur_credits;
		}
		if (state->conn->smb2.max_credits >= state->conn->smb2.cur_credits) {
			credits += 1;
		}

		mid = state->conn->smb2.mid;
		state->conn->smb2.mid += charge;
		state->conn->smb2.cur_credits -= charge;

		if (state->conn->smb2.server.capabilities & SMB2_CAP_LARGE_MTU) {
			SSVAL(state->smb2.hdr, SMB2_HDR_CREDIT_CHARGE, charge);
		}
		SSVAL(state->smb2.hdr, SMB2_HDR_CREDIT, credits);
		SBVAL(state->smb2.hdr, SMB2_HDR_MESSAGE_ID, mid);

		state->smb2.cancel_flags = 0;
		state->smb2.cancel_mid = mid;
		state->smb2.cancel_aid = 0;

skip_credits:
		if (state->session && encryption_key == NULL) {
			/*
			 * We prefer the channel signing key if it is
			 * already there.
			 */
			if (state->smb2.should_sign) {
				signing_key = &state->session->smb2_channel.signing_key;
			}

			/*
			 * If it is a channel binding, we already have the main
			 * signing key and try that one.
			 */
			if (signing_key && signing_key->length == 0) {
				signing_key = &state->session->smb2->signing_key;
			}

			/*
			 * If we do not have any session key yet, we skip the
			 * signing of SMB2_OP_SESSSETUP requests.
			 */
			if (signing_key && signing_key->length == 0) {
				signing_key = NULL;
			}
		}

		hdr_iov = num_iov;
		iov[num_iov].iov_base = state->smb2.hdr;
		iov[num_iov].iov_len  = sizeof(state->smb2.hdr);
		num_iov += 1;

		iov[num_iov].iov_base = discard_const(state->smb2.fixed);
		iov[num_iov].iov_len  = state->smb2.fixed_len;
		num_iov += 1;

		if (state->smb2.dyn != NULL) {
			iov[num_iov].iov_base = discard_const(state->smb2.dyn);
			iov[num_iov].iov_len  = state->smb2.dyn_len;
			num_iov += 1;
		}

		reqlen  = sizeof(state->smb2.hdr);
		reqlen += state->smb2.fixed_len;
		reqlen += state->smb2.dyn_len;

		if (i < num_reqs-1) {
			if ((reqlen % 8) > 0) {
				uint8_t pad = 8 - (reqlen % 8);
				iov[num_iov].iov_base = state->smb2.pad;
				iov[num_iov].iov_len = pad;
				num_iov += 1;
				reqlen += pad;
			}
			SIVAL(state->smb2.hdr, SMB2_HDR_NEXT_COMMAND, reqlen);
		}

		state->smb2.encryption_session_id = encryption_session_id;

		if (signing_key != NULL) {
			NTSTATUS status;

			status = smb2_signing_sign_pdu(*signing_key,
						       state->session->conn->protocol,
						       &iov[hdr_iov], num_iov - hdr_iov);
			if (!NT_STATUS_IS_OK(status)) {
				return status;
			}
		}

		nbt_len += reqlen;

		ret = smbXcli_req_set_pending(reqs[i]);
		if (!ret) {
			return NT_STATUS_NO_MEMORY;
		}
	}

	state = tevent_req_data(reqs[0], struct smbXcli_req_state);
	_smb_setlen_tcp(state->length_hdr, nbt_len);
	iov[0].iov_base = state->length_hdr;
	iov[0].iov_len  = sizeof(state->length_hdr);

	if (encryption_key != NULL) {
		NTSTATUS status;
		size_t buflen = nbt_len - SMB2_TF_HDR_SIZE;
		uint8_t *buf;
		int vi;

		buf = talloc_array(iov, uint8_t, buflen);
		if (buf == NULL) {
			return NT_STATUS_NO_MEMORY;
		}

		/*
		 * We copy the buffers before encrypting them,
		 * this is at least currently needed for the
		 * to keep state->smb2.hdr.
		 *
		 * Also the callers may expect there buffers
		 * to be const.
		 */
		for (vi = tf_iov + 1; vi < num_iov; vi++) {
			struct iovec *v = &iov[vi];
			const uint8_t *o = (const uint8_t *)v->iov_base;

			memcpy(buf, o, v->iov_len);
			v->iov_base = (void *)buf;
			buf += v->iov_len;
		}

		status = smb2_signing_encrypt_pdu(*encryption_key,
					state->conn->smb2.server.cipher,
					&iov[tf_iov], num_iov - tf_iov);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
	}

	if (state->conn->dispatch_incoming == NULL) {
		state->conn->dispatch_incoming = smb2cli_conn_dispatch_incoming;
	}

	subreq = writev_send(state, state->ev, state->conn->outgoing,
			     state->conn->sock_fd, false, iov, num_iov);
	if (subreq == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	tevent_req_set_callback(subreq, smb2cli_req_writev_done, reqs[0]);
	state->write_req = subreq;

	return NT_STATUS_OK;
}
