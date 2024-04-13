static void smbXcli_negprot_smb2_done(struct tevent_req *subreq)
{
	struct tevent_req *req =
		tevent_req_callback_data(subreq,
		struct tevent_req);
	struct smbXcli_negprot_state *state =
		tevent_req_data(req,
		struct smbXcli_negprot_state);
	struct smbXcli_conn *conn = state->conn;
	size_t security_offset, security_length;
	DATA_BLOB blob;
	NTSTATUS status;
	struct iovec *iov;
	uint8_t *body;
	size_t i;
	uint16_t dialect_revision;
	struct smb2_negotiate_contexts c = { .num_contexts = 0, };
	uint32_t negotiate_context_offset = 0;
	uint16_t negotiate_context_count = 0;
	DATA_BLOB negotiate_context_blob = data_blob_null;
	size_t avail;
	size_t ctx_ofs;
	size_t needed;
	struct smb2_negotiate_context *preauth = NULL;
	uint16_t hash_count;
	uint16_t salt_length;
	uint16_t hash_selected;
	struct hc_sha512state sctx;
	struct smb2_negotiate_context *cipher = NULL;
	struct iovec sent_iov[3];
	static const struct smb2cli_req_expected_response expected[] = {
	{
		.status = NT_STATUS_OK,
		.body_size = 0x41
	}
	};

	status = smb2cli_req_recv(subreq, state, &iov,
				  expected, ARRAY_SIZE(expected));
	if (tevent_req_nterror(req, status)) {
		return;
	}

	body = (uint8_t *)iov[1].iov_base;

	dialect_revision = SVAL(body, 4);

	for (i=0; i < ARRAY_SIZE(smb2cli_prots); i++) {
		if (smb2cli_prots[i].proto < state->conn->min_protocol) {
			continue;
		}

		if (smb2cli_prots[i].proto > state->conn->max_protocol) {
			continue;
		}

		if (smb2cli_prots[i].smb2_dialect != dialect_revision) {
			continue;
		}

		conn->protocol = smb2cli_prots[i].proto;
		break;
	}

	if (conn->protocol == PROTOCOL_NONE) {
		TALLOC_FREE(subreq);

		if (state->conn->min_protocol >= PROTOCOL_SMB2_02) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		if (dialect_revision != SMB2_DIALECT_REVISION_2FF) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		/* make sure we do not loop forever */
		state->conn->min_protocol = PROTOCOL_SMB2_02;

		/*
		 * send a SMB2 negprot, in order to negotiate
		 * the SMB2 dialect.
		 */
		subreq = smbXcli_negprot_smb2_subreq(state);
		if (tevent_req_nomem(subreq, req)) {
			return;
		}
		tevent_req_set_callback(subreq, smbXcli_negprot_smb2_done, req);
		return;
	}

	conn->smb2.server.security_mode = SVAL(body, 2);
	if (conn->protocol >= PROTOCOL_SMB3_10) {
		negotiate_context_count = SVAL(body, 6);
	}

	blob = data_blob_const(body + 8, 16);
	status = GUID_from_data_blob(&blob, &conn->smb2.server.guid);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	conn->smb2.server.capabilities	= IVAL(body, 24);
	conn->smb2.server.max_trans_size= IVAL(body, 28);
	conn->smb2.server.max_read_size	= IVAL(body, 32);
	conn->smb2.server.max_write_size= IVAL(body, 36);
	conn->smb2.server.system_time	= BVAL(body, 40);
	conn->smb2.server.start_time	= BVAL(body, 48);

	security_offset = SVAL(body, 56);
	security_length = SVAL(body, 58);

	if (security_offset != SMB2_HDR_BODY + iov[1].iov_len) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (security_length > iov[2].iov_len) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	conn->smb2.server.gss_blob = data_blob_talloc(conn,
						iov[2].iov_base,
						security_length);
	if (tevent_req_nomem(conn->smb2.server.gss_blob.data, req)) {
		return;
	}

	if (conn->protocol < PROTOCOL_SMB3_10) {
		TALLOC_FREE(subreq);

		if (conn->smb2.server.capabilities & SMB2_CAP_ENCRYPTION) {
			conn->smb2.server.cipher = SMB2_ENCRYPTION_AES128_CCM;
		}
		tevent_req_done(req);
		return;
	}

	if (conn->smb2.server.capabilities & SMB2_CAP_ENCRYPTION) {
		tevent_req_nterror(req,
				NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	negotiate_context_offset = IVAL(body, 60);
	if (negotiate_context_offset < security_offset) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	ctx_ofs = negotiate_context_offset - security_offset;
	if (ctx_ofs > iov[2].iov_len) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}
	avail = iov[2].iov_len - security_length;
	needed = iov[2].iov_len - ctx_ofs;
	if (needed > avail) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	negotiate_context_blob.data = (uint8_t *)iov[2].iov_base;
	negotiate_context_blob.length = iov[2].iov_len;

	negotiate_context_blob.data += ctx_ofs;
	negotiate_context_blob.length -= ctx_ofs;

	status = smb2_negotiate_context_parse(state, negotiate_context_blob, &c);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	if (negotiate_context_count != c.num_contexts) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	preauth = smb2_negotiate_context_find(&c,
					SMB2_PREAUTH_INTEGRITY_CAPABILITIES);
	if (preauth == NULL) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (preauth->data.length < 6) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	hash_count = SVAL(preauth->data.data, 0);
	salt_length = SVAL(preauth->data.data, 2);
	hash_selected = SVAL(preauth->data.data, 4);

	if (hash_count != 1) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (preauth->data.length != (6 + salt_length)) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (hash_selected != SMB2_PREAUTH_INTEGRITY_SHA512) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	cipher = smb2_negotiate_context_find(&c, SMB2_ENCRYPTION_CAPABILITIES);
	if (cipher != NULL) {
		uint16_t cipher_count;

		if (cipher->data.length < 2) {
			tevent_req_nterror(req,
					NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		cipher_count = SVAL(cipher->data.data, 0);

		if (cipher_count > 1) {
			tevent_req_nterror(req,
					NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		if (cipher->data.length != (2 + 2 * cipher_count)) {
			tevent_req_nterror(req,
					NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		if (cipher_count == 1) {
			uint16_t cipher_selected;

			cipher_selected = SVAL(cipher->data.data, 2);

			switch (cipher_selected) {
			case SMB2_ENCRYPTION_AES128_GCM:
			case SMB2_ENCRYPTION_AES128_CCM:
				conn->smb2.server.cipher = cipher_selected;
				break;
			}
		}
	}

	/* First we hash the request */
	smb2cli_req_get_sent_iov(subreq, sent_iov);
	samba_SHA512_Init(&sctx);
	samba_SHA512_Update(&sctx, conn->smb2.preauth_sha512,
		      sizeof(conn->smb2.preauth_sha512));
	for (i = 0; i < 3; i++) {
		samba_SHA512_Update(&sctx, sent_iov[i].iov_base, sent_iov[i].iov_len);
	}
	samba_SHA512_Final(conn->smb2.preauth_sha512, &sctx);
	TALLOC_FREE(subreq);

	/* And now we hash the response */
	samba_SHA512_Init(&sctx);
	samba_SHA512_Update(&sctx, conn->smb2.preauth_sha512,
		      sizeof(conn->smb2.preauth_sha512));
	for (i = 0; i < 3; i++) {
		samba_SHA512_Update(&sctx, iov[i].iov_base, iov[i].iov_len);
	}
	samba_SHA512_Final(conn->smb2.preauth_sha512, &sctx);

	tevent_req_done(req);
}
