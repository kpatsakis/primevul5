auth_server_input_mech(struct auth_server_connection *conn,
		       const char *const *args)
{
	struct auth_mech_desc mech_desc;

	if (conn->handshake_received) {
		i_error("BUG: Authentication server already sent handshake");
		return -1;
	}
	if (args[0] == NULL) {
		i_error("BUG: Authentication server sent broken MECH line");
		return -1;
	}

	i_zero(&mech_desc);
	mech_desc.name = p_strdup(conn->pool, args[0]);

	if (strcmp(mech_desc.name, "PLAIN") == 0)
		conn->has_plain_mech = TRUE;

	for (args++; *args != NULL; args++) {
		if (strcmp(*args, "private") == 0)
			mech_desc.flags |= MECH_SEC_PRIVATE;
		else if (strcmp(*args, "anonymous") == 0)
			mech_desc.flags |= MECH_SEC_ANONYMOUS;
		else if (strcmp(*args, "plaintext") == 0)
			mech_desc.flags |= MECH_SEC_PLAINTEXT;
		else if (strcmp(*args, "dictionary") == 0)
			mech_desc.flags |= MECH_SEC_DICTIONARY;
		else if (strcmp(*args, "active") == 0)
			mech_desc.flags |= MECH_SEC_ACTIVE;
		else if (strcmp(*args, "forward-secrecy") == 0)
			mech_desc.flags |= MECH_SEC_FORWARD_SECRECY;
		else if (strcmp(*args, "mutual-auth") == 0)
			mech_desc.flags |= MECH_SEC_MUTUAL_AUTH;
	}
	array_append(&conn->available_auth_mechs, &mech_desc, 1);
	return 0;
}