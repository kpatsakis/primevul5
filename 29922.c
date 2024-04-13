static NTSTATUS smb2cli_conn_dispatch_incoming(struct smbXcli_conn *conn,
					       TALLOC_CTX *tmp_mem,
					       uint8_t *inbuf)
{
	struct tevent_req *req;
	struct smbXcli_req_state *state = NULL;
	struct iovec *iov;
	int i, num_iov;
	NTSTATUS status;
	bool defer = true;
	struct smbXcli_session *last_session = NULL;
	size_t inbuf_len = smb_len_tcp(inbuf);

	status = smb2cli_inbuf_parse_compound(conn,
					      inbuf + NBT_HDR_SIZE,
					      inbuf_len,
					      tmp_mem,
					      &iov, &num_iov);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	for (i=0; i<num_iov; i+=4) {
		uint8_t *inbuf_ref = NULL;
		struct iovec *cur = &iov[i];
		uint8_t *inhdr = (uint8_t *)cur[1].iov_base;
		uint16_t opcode = SVAL(inhdr, SMB2_HDR_OPCODE);
		uint32_t flags = IVAL(inhdr, SMB2_HDR_FLAGS);
		uint64_t mid = BVAL(inhdr, SMB2_HDR_MESSAGE_ID);
		uint16_t req_opcode;
		uint32_t req_flags;
		uint16_t credits = SVAL(inhdr, SMB2_HDR_CREDIT);
		uint32_t new_credits;
		struct smbXcli_session *session = NULL;
		const DATA_BLOB *signing_key = NULL;
		bool was_encrypted = false;

		new_credits = conn->smb2.cur_credits;
		new_credits += credits;
		if (new_credits > UINT16_MAX) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}
		conn->smb2.cur_credits += credits;

		req = smb2cli_conn_find_pending(conn, mid);
		if (req == NULL) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}
		state = tevent_req_data(req, struct smbXcli_req_state);

		req_opcode = SVAL(state->smb2.hdr, SMB2_HDR_OPCODE);
		if (opcode != req_opcode) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}
		req_flags = SVAL(state->smb2.hdr, SMB2_HDR_FLAGS);

		if (!(flags & SMB2_HDR_FLAG_REDIRECT)) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}

		status = NT_STATUS(IVAL(inhdr, SMB2_HDR_STATUS));
		if ((flags & SMB2_HDR_FLAG_ASYNC) &&
		    NT_STATUS_EQUAL(status, STATUS_PENDING)) {
			uint64_t async_id = BVAL(inhdr, SMB2_HDR_ASYNC_ID);

			if (state->smb2.got_async) {
				/* We only expect one STATUS_PENDING response */
				return NT_STATUS_INVALID_NETWORK_RESPONSE;
			}
			state->smb2.got_async = true;

			/*
			 * async interim responses are not signed,
			 * even if the SMB2_HDR_FLAG_SIGNED flag
			 * is set.
			 */
			state->smb2.cancel_flags = SMB2_HDR_FLAG_ASYNC;
			state->smb2.cancel_mid = 0;
			state->smb2.cancel_aid = async_id;

			if (state->smb2.notify_async) {
				tevent_req_defer_callback(req, state->ev);
				tevent_req_notify_callback(req);
			}
			continue;
		}

		session = state->session;
		if (req_flags & SMB2_HDR_FLAG_CHAINED) {
			session = last_session;
		}
		last_session = session;

		if (state->smb2.should_sign) {
			if (!(flags & SMB2_HDR_FLAG_SIGNED)) {
				return NT_STATUS_ACCESS_DENIED;
			}
		}

		if (flags & SMB2_HDR_FLAG_SIGNED) {
			uint64_t uid = BVAL(inhdr, SMB2_HDR_SESSION_ID);

			if (session == NULL) {
				struct smbXcli_session *s;

				s = state->conn->sessions;
				for (; s; s = s->next) {
					if (s->smb2->session_id != uid) {
						continue;
					}

					session = s;
					break;
				}
			}

			if (session == NULL) {
				return NT_STATUS_INVALID_NETWORK_RESPONSE;
			}

			last_session = session;
			signing_key = &session->smb2_channel.signing_key;
		}

		if (opcode == SMB2_OP_SESSSETUP) {
			/*
			 * We prefer the channel signing key, if it is
			 * already there.
			 *
			 * If we do not have a channel signing key yet,
			 * we try the main signing key, if it is not
			 * the final response.
			 */
			if (signing_key && signing_key->length == 0 &&
			    !NT_STATUS_IS_OK(status)) {
				signing_key = &session->smb2->signing_key;
			}

			if (signing_key && signing_key->length == 0) {
				/*
				 * If we do not have a session key to
				 * verify the signature, we defer the
				 * signing check to the caller.
				 *
				 * The caller gets NT_STATUS_OK, it
				 * has to call
				 * smb2cli_session_set_session_key()
				 * or
				 * smb2cli_session_set_channel_key()
				 * which will check the signature
				 * with the channel signing key.
				 */
				signing_key = NULL;
			}
		}

		if (cur[0].iov_len == SMB2_TF_HDR_SIZE) {
			const uint8_t *tf = (const uint8_t *)cur[0].iov_base;
			uint64_t uid = BVAL(tf, SMB2_TF_SESSION_ID);

			/*
			 * If the response was encrypted in a SMB2_TRANSFORM
			 * pdu, which belongs to the correct session,
			 * we do not need to do signing checks
			 *
			 * It could be the session the response belongs to
			 * or the session that was used to encrypt the
			 * SMB2_TRANSFORM request.
			 */
			if ((session && session->smb2->session_id == uid) ||
			    (state->smb2.encryption_session_id == uid)) {
				signing_key = NULL;
				was_encrypted = true;
			}
		}

		if (NT_STATUS_EQUAL(status, NT_STATUS_USER_SESSION_DELETED)) {
			/*
			 * if the server returns NT_STATUS_USER_SESSION_DELETED
			 * the response is not signed and we should
			 * propagate the NT_STATUS_USER_SESSION_DELETED
			 * status to the caller.
			 */
			state->smb2.signing_skipped = true;
			signing_key = NULL;
		}

		if (NT_STATUS_EQUAL(status, NT_STATUS_INVALID_PARAMETER)) {
			/*
			 * if the server returns
			 * NT_STATUS_INVALID_PARAMETER
			 * the response might not be encrypted.
			 */
			if (state->smb2.should_encrypt && !was_encrypted) {
				state->smb2.signing_skipped = true;
				signing_key = NULL;
			}
		}

		if (state->smb2.should_encrypt && !was_encrypted) {
			if (!state->smb2.signing_skipped) {
				return NT_STATUS_ACCESS_DENIED;
			}
		}

		if (NT_STATUS_EQUAL(status, NT_STATUS_NETWORK_NAME_DELETED) ||
		    NT_STATUS_EQUAL(status, NT_STATUS_FILE_CLOSED) ||
		    NT_STATUS_EQUAL(status, NT_STATUS_INVALID_PARAMETER)) {
			/*
			 * if the server returns
			 * NT_STATUS_NETWORK_NAME_DELETED
			 * NT_STATUS_FILE_CLOSED
			 * NT_STATUS_INVALID_PARAMETER
			 * the response might not be signed
			 * as this happens before the signing checks.
			 *
			 * If server echos the signature (or all zeros)
			 * we should report the status from the server
			 * to the caller.
			 */
			if (signing_key) {
				int cmp;

				cmp = memcmp(inhdr+SMB2_HDR_SIGNATURE,
					     state->smb2.hdr+SMB2_HDR_SIGNATURE,
					     16);
				if (cmp == 0) {
					state->smb2.signing_skipped = true;
					signing_key = NULL;
				}
			}
			if (signing_key) {
				int cmp;
				static const uint8_t zeros[16];

				cmp = memcmp(inhdr+SMB2_HDR_SIGNATURE,
					     zeros,
					     16);
				if (cmp == 0) {
					state->smb2.signing_skipped = true;
					signing_key = NULL;
				}
			}
		}

		if (signing_key) {
			status = smb2_signing_check_pdu(*signing_key,
							state->conn->protocol,
							&cur[1], 3);
			if (!NT_STATUS_IS_OK(status)) {
				/*
				 * If the signing check fails, we disconnect
				 * the connection.
				 */
				return status;
			}
		}

		if (NT_STATUS_EQUAL(status, NT_STATUS_NETWORK_SESSION_EXPIRED) &&
		    (session != NULL) && session->disconnect_expired)
		{
			/*
			 * this should be a short term hack
			 * until the upper layers have implemented
			 * re-authentication.
			 */
			return status;
		}

		smbXcli_req_unset_pending(req);

		/*
		 * There might be more than one response
		 * we need to defer the notifications
		 */
		if ((num_iov == 5) && (talloc_array_length(conn->pending) == 0)) {
			defer = false;
		}

		if (defer) {
			tevent_req_defer_callback(req, state->ev);
		}

		/*
		 * Note: here we use talloc_reference() in a way
		 *       that does not expose it to the caller.
		 */
		inbuf_ref = talloc_reference(state->smb2.recv_iov, inbuf);
		if (tevent_req_nomem(inbuf_ref, req)) {
			continue;
		}

		/* copy the related buffers */
		state->smb2.recv_iov[0] = cur[1];
		state->smb2.recv_iov[1] = cur[2];
		state->smb2.recv_iov[2] = cur[3];

		tevent_req_done(req);
	}

	if (defer) {
		return NT_STATUS_RETRY;
	}

	return NT_STATUS_OK;
}
