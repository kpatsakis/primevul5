static void smb2cli_validate_negotiate_info_done(struct tevent_req *subreq)
{
	struct tevent_req *req =
		tevent_req_callback_data(subreq,
		struct tevent_req);
	struct smb2cli_validate_negotiate_info_state *state =
		tevent_req_data(req,
		struct smb2cli_validate_negotiate_info_state);
	NTSTATUS status;
	const uint8_t *buf;
	uint32_t capabilities;
	DATA_BLOB guid_blob;
	struct GUID server_guid;
	uint16_t security_mode;
	uint16_t dialect;

	status = smb2cli_ioctl_recv(subreq, state,
				    &state->out_input_buffer,
				    &state->out_output_buffer);
	TALLOC_FREE(subreq);
	if (NT_STATUS_EQUAL(status, NT_STATUS_FILE_CLOSED)) {
		/*
		 * The response was signed, but not supported
		 *
		 * Older Windows and Samba releases return
		 * NT_STATUS_FILE_CLOSED.
		 */
		tevent_req_done(req);
		return;
	}
	if (NT_STATUS_EQUAL(status, NT_STATUS_INVALID_DEVICE_REQUEST)) {
		/*
		 * The response was signed, but not supported
		 *
		 * This is returned by the NTVFS based Samba 4.x file server
		 * for file shares.
		 */
		tevent_req_done(req);
		return;
	}
	if (NT_STATUS_EQUAL(status, NT_STATUS_FS_DRIVER_REQUIRED)) {
		/*
		 * The response was signed, but not supported
		 *
		 * This is returned by the NTVFS based Samba 4.x file server
		 * for ipc shares.
		 */
		tevent_req_done(req);
		return;
	}
	if (tevent_req_nterror(req, status)) {
		return;
	}

	if (state->out_output_buffer.length != 24) {
		tevent_req_nterror(req, NT_STATUS_INVALID_NETWORK_RESPONSE);
		return;
	}

	buf = state->out_output_buffer.data;

	capabilities = IVAL(buf, 0);
	guid_blob = data_blob_const(buf + 4, 16);
	status = GUID_from_data_blob(&guid_blob, &server_guid);
	if (tevent_req_nterror(req, status)) {
		return;
	}
	security_mode = CVAL(buf, 20);
	dialect = SVAL(buf, 22);

	if (capabilities != state->conn->smb2.server.capabilities) {
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (!GUID_equal(&server_guid, &state->conn->smb2.server.guid)) {
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (security_mode != state->conn->smb2.server.security_mode) {
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if (dialect != state->dialect) {
		tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	tevent_req_done(req);
}
