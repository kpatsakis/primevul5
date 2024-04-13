struct smbXcli_conn *smbXcli_conn_create(TALLOC_CTX *mem_ctx,
					 int fd,
					 const char *remote_name,
					 enum smb_signing_setting signing_state,
					 uint32_t smb1_capabilities,
					 struct GUID *client_guid,
					 uint32_t smb2_capabilities)
{
	struct smbXcli_conn *conn = NULL;
	void *ss = NULL;
	struct sockaddr *sa = NULL;
	socklen_t sa_length;
	int ret;

	conn = talloc_zero(mem_ctx, struct smbXcli_conn);
	if (!conn) {
		return NULL;
	}

	conn->sock_fd = fd;

	conn->remote_name = talloc_strdup(conn, remote_name);
	if (conn->remote_name == NULL) {
		goto error;
	}

	ss = (void *)&conn->local_ss;
	sa = (struct sockaddr *)ss;
	sa_length = sizeof(conn->local_ss);
	ret = getsockname(fd, sa, &sa_length);
	if (ret == -1) {
		goto error;
	}
	ss = (void *)&conn->remote_ss;
	sa = (struct sockaddr *)ss;
	sa_length = sizeof(conn->remote_ss);
	ret = getpeername(fd, sa, &sa_length);
	if (ret == -1) {
		goto error;
	}

	conn->outgoing = tevent_queue_create(conn, "smbXcli_outgoing");
	if (conn->outgoing == NULL) {
		goto error;
	}
	conn->pending = NULL;

	conn->min_protocol = PROTOCOL_NONE;
	conn->max_protocol = PROTOCOL_NONE;
	conn->protocol = PROTOCOL_NONE;

	switch (signing_state) {
	case SMB_SIGNING_OFF:
		/* never */
		conn->allow_signing = false;
		conn->desire_signing = false;
		conn->mandatory_signing = false;
		break;
	case SMB_SIGNING_DEFAULT:
	case SMB_SIGNING_IF_REQUIRED:
		/* if the server requires it */
		conn->allow_signing = true;
		conn->desire_signing = false;
		conn->mandatory_signing = false;
		break;
	case SMB_SIGNING_DESIRED:
		/* if the server desires it */
		conn->allow_signing = true;
		conn->desire_signing = true;
		conn->mandatory_signing = false;
		break;
	case SMB_SIGNING_REQUIRED:
		/* always */
		conn->allow_signing = true;
		conn->desire_signing = true;
		conn->mandatory_signing = true;
		break;
	}

	conn->smb1.client.capabilities = smb1_capabilities;
	conn->smb1.client.max_xmit = UINT16_MAX;

	conn->smb1.capabilities = conn->smb1.client.capabilities;
	conn->smb1.max_xmit = 1024;

	conn->smb1.mid = 1;

	/* initialise signing */
	conn->smb1.signing = smb_signing_init(conn,
					      conn->allow_signing,
					      conn->desire_signing,
					      conn->mandatory_signing);
	if (!conn->smb1.signing) {
		goto error;
	}

	conn->smb2.client.security_mode = SMB2_NEGOTIATE_SIGNING_ENABLED;
	if (conn->mandatory_signing) {
		conn->smb2.client.security_mode |= SMB2_NEGOTIATE_SIGNING_REQUIRED;
	}
	if (client_guid) {
		conn->smb2.client.guid = *client_guid;
	}
	conn->smb2.client.capabilities = smb2_capabilities;

	conn->smb2.cur_credits = 1;
	conn->smb2.max_credits = 0;
	conn->smb2.io_priority = 1;

	/*
	 * Samba and Windows servers accept a maximum of 16 MiB with a maximum
	 * chunk length of 1 MiB.
	 */
	conn->smb2.cc_chunk_len = 1024 * 1024;
	conn->smb2.cc_max_chunks = 16;

	talloc_set_destructor(conn, smbXcli_conn_destructor);
	return conn;

 error:
	TALLOC_FREE(conn);
	return NULL;
}
