static void smbXcli_negprot_smb1_done(struct tevent_req *subreq)
{
	struct tevent_req *req =
		tevent_req_callback_data(subreq,
		struct tevent_req);
	struct smbXcli_negprot_state *state =
		tevent_req_data(req,
		struct smbXcli_negprot_state);
	struct smbXcli_conn *conn = state->conn;
	struct iovec *recv_iov = NULL;
	uint8_t *inhdr;
	uint8_t wct;
	uint16_t *vwv;
	uint32_t num_bytes;
	uint8_t *bytes;
	NTSTATUS status;
	uint16_t protnum;
	size_t i;
	size_t num_prots = 0;
	uint8_t flags;
	uint32_t client_capabilities = conn->smb1.client.capabilities;
	uint32_t both_capabilities;
	uint32_t server_capabilities = 0;
	uint32_t capabilities;
	uint32_t client_max_xmit = conn->smb1.client.max_xmit;
	uint32_t server_max_xmit = 0;
	uint32_t max_xmit;
	uint32_t server_max_mux = 0;
	uint16_t server_security_mode = 0;
	uint32_t server_session_key = 0;
	bool server_readbraw = false;
	bool server_writebraw = false;
	bool server_lockread = false;
	bool server_writeunlock = false;
	struct GUID server_guid = GUID_zero();
	DATA_BLOB server_gss_blob = data_blob_null;
	uint8_t server_challenge[8];
	char *server_workgroup = NULL;
	char *server_name = NULL;
	int server_time_zone = 0;
	NTTIME server_system_time = 0;
	static const struct smb1cli_req_expected_response expected[] = {
	{
		.status = NT_STATUS_OK,
		.wct = 0x11, /* NT1 */
	},
	{
		.status = NT_STATUS_OK,
		.wct = 0x0D, /* LM */
	},
	{
		.status = NT_STATUS_OK,
		.wct = 0x01, /* CORE */
	}
	};

	ZERO_STRUCT(server_challenge);

	status = smb1cli_req_recv(subreq, state,
				  &recv_iov,
				  &inhdr,
				  &wct,
				  &vwv,
				  NULL, /* pvwv_offset */
				  &num_bytes,
				  &bytes,
				  NULL, /* pbytes_offset */
				  NULL, /* pinbuf */
				  expected, ARRAY_SIZE(expected));
	TALLOC_FREE(subreq);
	if (tevent_req_nterror(req, status)) {
		return;
	}

	flags = CVAL(inhdr, HDR_FLG);

	protnum = SVAL(vwv, 0);

	for (i=0; i < ARRAY_SIZE(smb1cli_prots); i++) {
		if (smb1cli_prots[i].proto < state->conn->min_protocol) {
			continue;
		}

		if (smb1cli_prots[i].proto > state->conn->max_protocol) {
			continue;
		}

		if (protnum != num_prots) {
			num_prots++;
			continue;
		}

		conn->protocol = smb1cli_prots[i].proto;
		break;
	}

	if (conn->protocol == PROTOCOL_NONE) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if ((conn->protocol < PROTOCOL_NT1) && conn->mandatory_signing) {
		DEBUG(0,("smbXcli_negprot: SMB signing is mandatory "
			 "and the selected protocol level doesn't support it.\n"));
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (flags & FLAG_SUPPORT_LOCKREAD) {
		server_lockread = true;
		server_writeunlock = true;
	}

	if (conn->protocol >= PROTOCOL_NT1) {
		const char *client_signing = NULL;
		bool server_mandatory = false;
		bool server_allowed = false;
		const char *server_signing = NULL;
		bool ok;
		uint8_t key_len;

		if (wct != 0x11) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		/* NT protocol */
		server_security_mode = CVAL(vwv + 1, 0);
		server_max_mux = SVAL(vwv + 1, 1);
		server_max_xmit = IVAL(vwv + 3, 1);
		server_session_key = IVAL(vwv + 7, 1);
		server_time_zone = SVALS(vwv + 15, 1);
		server_time_zone *= 60;
		/* this time arrives in real GMT */
		server_system_time = BVAL(vwv + 11, 1);
		server_capabilities = IVAL(vwv + 9, 1);

		key_len = CVAL(vwv + 16, 1);

		if (server_capabilities & CAP_RAW_MODE) {
			server_readbraw = true;
			server_writebraw = true;
		}
		if (server_capabilities & CAP_LOCK_AND_READ) {
			server_lockread = true;
		}

		if (server_capabilities & CAP_EXTENDED_SECURITY) {
			DATA_BLOB blob1, blob2;

			if (num_bytes < 16) {
				tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
				return;
			}

			blob1 = data_blob_const(bytes, 16);
			status = GUID_from_data_blob(&blob1, &server_guid);
			if (tevent_req_nterror(req, status)) {
				return;
			}

			blob1 = data_blob_const(bytes+16, num_bytes-16);
			blob2 = data_blob_dup_talloc(state, blob1);
			if (blob1.length > 0 &&
			    tevent_req_nomem(blob2.data, req)) {
				return;
			}
			server_gss_blob = blob2;
		} else {
			DATA_BLOB blob1, blob2;

			if (num_bytes < key_len) {
				tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
				return;
			}

			if (key_len != 0 && key_len != 8) {
				tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
				return;
			}

			if (key_len == 8) {
				memcpy(server_challenge, bytes, 8);
			}

			blob1 = data_blob_const(bytes+key_len, num_bytes-key_len);
			blob2 = data_blob_const(bytes+key_len, num_bytes-key_len);
			if (blob1.length > 0) {
				size_t len;

				len = utf16_len_n(blob1.data,
						  blob1.length);
				blob1.length = len;

				ok = convert_string_talloc(state,
							   CH_UTF16LE,
							   CH_UNIX,
							   blob1.data,
							   blob1.length,
							   &server_workgroup,
							   &len);
				if (!ok) {
					status = map_nt_error_from_unix_common(errno);
					tevent_req_nterror(req, status);
					return;
				}
			}

			blob2.data += blob1.length;
			blob2.length -= blob1.length;
			if (blob2.length > 0) {
				size_t len;

				len = utf16_len_n(blob1.data,
						  blob1.length);
				blob1.length = len;

				ok = convert_string_talloc(state,
							   CH_UTF16LE,
							   CH_UNIX,
							   blob2.data,
							   blob2.length,
							   &server_name,
							   &len);
				if (!ok) {
					status = map_nt_error_from_unix_common(errno);
					tevent_req_nterror(req, status);
					return;
				}
			}
		}

		client_signing = "disabled";
		if (conn->allow_signing) {
			client_signing = "allowed";
		}
		if (conn->mandatory_signing) {
			client_signing = "required";
		}

		server_signing = "not supported";
		if (server_security_mode & NEGOTIATE_SECURITY_SIGNATURES_ENABLED) {
			server_signing = "supported";
			server_allowed = true;
		} else if (conn->mandatory_signing) {
			/*
			 * We have mandatory signing as client
			 * lets assume the server will look at our
			 * FLAGS2_SMB_SECURITY_SIGNATURES_REQUIRED
			 * flag in the session setup
			 */
			server_signing = "not announced";
			server_allowed = true;
		}
		if (server_security_mode & NEGOTIATE_SECURITY_SIGNATURES_REQUIRED) {
			server_signing = "required";
			server_mandatory = true;
		}

		ok = smb_signing_set_negotiated(conn->smb1.signing,
						server_allowed,
						server_mandatory);
		if (!ok) {
			DEBUG(1,("cli_negprot: SMB signing is required, "
				 "but client[%s] and server[%s] mismatch\n",
				 client_signing, server_signing));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return;
		}

	} else if (conn->protocol >= PROTOCOL_LANMAN1) {
		DATA_BLOB blob1;
		uint8_t key_len;
		time_t t;

		if (wct != 0x0D) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		server_security_mode = SVAL(vwv + 1, 0);
		server_max_xmit = SVAL(vwv + 2, 0);
		server_max_mux = SVAL(vwv + 3, 0);
		server_readbraw = ((SVAL(vwv + 5, 0) & 0x1) != 0);
		server_writebraw = ((SVAL(vwv + 5, 0) & 0x2) != 0);
		server_session_key = IVAL(vwv + 6, 0);
		server_time_zone = SVALS(vwv + 10, 0);
		server_time_zone *= 60;
		/* this time is converted to GMT by make_unix_date */
		t = pull_dos_date((const uint8_t *)(vwv + 8), server_time_zone);
		unix_to_nt_time(&server_system_time, t);
		key_len = SVAL(vwv + 11, 0);

		if (num_bytes < key_len) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		if (key_len != 0 && key_len != 8) {
			tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
			return;
		}

		if (key_len == 8) {
			memcpy(server_challenge, bytes, 8);
		}

		blob1 = data_blob_const(bytes+key_len, num_bytes-key_len);
		if (blob1.length > 0) {
			size_t len;
			bool ok;

			len = utf16_len_n(blob1.data,
					  blob1.length);
			blob1.length = len;

			ok = convert_string_talloc(state,
						   CH_DOS,
						   CH_UNIX,
						   blob1.data,
						   blob1.length,
						   &server_workgroup,
						   &len);
			if (!ok) {
				status = map_nt_error_from_unix_common(errno);
				tevent_req_nterror(req, status);
				return;
			}
		}

	} else {
		/* the old core protocol */
		server_time_zone = get_time_zone(time(NULL));
		server_max_xmit = 1024;
		server_max_mux = 1;
	}

	if (server_max_xmit < 1024) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	if (server_max_mux < 1) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	/*
	 * Now calculate the negotiated capabilities
	 * based on the mask for:
	 * - client only flags
	 * - flags used in both directions
	 * - server only flags
	 */
	both_capabilities = client_capabilities & server_capabilities;
	capabilities = client_capabilities & SMB_CAP_CLIENT_MASK;
	capabilities |= both_capabilities & SMB_CAP_BOTH_MASK;
	capabilities |= server_capabilities & SMB_CAP_SERVER_MASK;

	max_xmit = MIN(client_max_xmit, server_max_xmit);

	conn->smb1.server.capabilities = server_capabilities;
	conn->smb1.capabilities = capabilities;

	conn->smb1.server.max_xmit = server_max_xmit;
	conn->smb1.max_xmit = max_xmit;

	conn->smb1.server.max_mux = server_max_mux;

	conn->smb1.server.security_mode = server_security_mode;

	conn->smb1.server.readbraw = server_readbraw;
	conn->smb1.server.writebraw = server_writebraw;
	conn->smb1.server.lockread = server_lockread;
	conn->smb1.server.writeunlock = server_writeunlock;

	conn->smb1.server.session_key = server_session_key;

	talloc_steal(conn, server_gss_blob.data);
	conn->smb1.server.gss_blob = server_gss_blob;
	conn->smb1.server.guid = server_guid;
	memcpy(conn->smb1.server.challenge, server_challenge, 8);
	conn->smb1.server.workgroup = talloc_move(conn, &server_workgroup);
	conn->smb1.server.name = talloc_move(conn, &server_name);

	conn->smb1.server.time_zone = server_time_zone;
	conn->smb1.server.system_time = server_system_time;

	tevent_req_done(req);
}
