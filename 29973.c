static struct tevent_req *smbXcli_negprot_smb2_subreq(struct smbXcli_negprot_state *state)
{
	size_t i;
	uint8_t *buf;
	uint16_t dialect_count = 0;
	DATA_BLOB dyn = data_blob_null;

	for (i=0; i < ARRAY_SIZE(smb2cli_prots); i++) {
		bool ok;
		uint8_t val[2];

		if (smb2cli_prots[i].proto < state->conn->min_protocol) {
			continue;
		}

		if (smb2cli_prots[i].proto > state->conn->max_protocol) {
			continue;
		}

		SSVAL(val, 0, smb2cli_prots[i].smb2_dialect);

		ok = data_blob_append(state, &dyn, val, sizeof(val));
		if (!ok) {
			return NULL;
		}

		dialect_count++;
	}

	buf = state->smb2.fixed;
	SSVAL(buf, 0, 36);
	SSVAL(buf, 2, dialect_count);
	SSVAL(buf, 4, state->conn->smb2.client.security_mode);
	SSVAL(buf, 6, 0);	/* Reserved */
	if (state->conn->max_protocol >= PROTOCOL_SMB2_22) {
		SIVAL(buf, 8, state->conn->smb2.client.capabilities);
	} else {
		SIVAL(buf, 8, 0); 	/* Capabilities */
	}
	if (state->conn->max_protocol >= PROTOCOL_SMB2_10) {
		NTSTATUS status;
		DATA_BLOB blob;

		status = GUID_to_ndr_blob(&state->conn->smb2.client.guid,
					  state, &blob);
		if (!NT_STATUS_IS_OK(status)) {
			return NULL;
		}
		memcpy(buf+12, blob.data, 16); /* ClientGuid */
	} else {
		memset(buf+12, 0, 16);	/* ClientGuid */
	}

	if (state->conn->max_protocol >= PROTOCOL_SMB3_10) {
		NTSTATUS status;
		struct smb2_negotiate_contexts c = { .num_contexts = 0, };
		uint32_t offset;
		DATA_BLOB b;
		uint8_t p[38];
		const uint8_t zeros[8] = {0, };
		size_t pad;
		bool ok;

		SSVAL(p, 0,  1); /* HashAlgorithmCount */
		SSVAL(p, 2, 32); /* SaltLength */
		SSVAL(p, 4, SMB2_PREAUTH_INTEGRITY_SHA512);
		generate_random_buffer(p + 6, 32);

		b = data_blob_const(p, 38);
		status = smb2_negotiate_context_add(state, &c,
					SMB2_PREAUTH_INTEGRITY_CAPABILITIES, b);
		if (!NT_STATUS_IS_OK(status)) {
			return NULL;
		}

		SSVAL(p, 0, 2); /* ChiperCount */
		/*
		 * For now we preferr CCM because our implementation
		 * is faster than GCM, see bug #11451.
		 */
		SSVAL(p, 2, SMB2_ENCRYPTION_AES128_CCM);
		SSVAL(p, 4, SMB2_ENCRYPTION_AES128_GCM);

		b = data_blob_const(p, 6);
		status = smb2_negotiate_context_add(state, &c,
					SMB2_ENCRYPTION_CAPABILITIES, b);
		if (!NT_STATUS_IS_OK(status)) {
			return NULL;
		}

		status = smb2_negotiate_context_push(state, &b, c);
		if (!NT_STATUS_IS_OK(status)) {
			return NULL;
		}

		offset = SMB2_HDR_BODY + sizeof(state->smb2.fixed) + dyn.length;
		pad = smbXcli_padding_helper(offset, 8);

		ok = data_blob_append(state, &dyn, zeros, pad);
		if (!ok) {
			return NULL;
		}
		offset += pad;

		ok = data_blob_append(state, &dyn, b.data, b.length);
		if (!ok) {
			return NULL;
		}

		SIVAL(buf, 28, offset);   /* NegotiateContextOffset */
		SSVAL(buf, 32, c.num_contexts); /* NegotiateContextCount */
		SSVAL(buf, 34, 0);        /* Reserved */
	} else {
		SBVAL(buf, 28, 0);	/* Reserved/ClientStartTime */
	}

	return smb2cli_req_send(state, state->ev,
				state->conn, SMB2_OP_NEGPROT,
				0, 0, /* flags */
				state->timeout_msec,
				NULL, NULL, /* tcon, session */
				state->smb2.fixed, sizeof(state->smb2.fixed),
				dyn.data, dyn.length,
				UINT16_MAX); /* max_dyn_len */
}
