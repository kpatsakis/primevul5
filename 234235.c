static void auth_server_connection_input(struct auth_server_connection *conn)
{
	struct istream *input;
	const char *line, *error;
	int ret;

	switch (i_stream_read(conn->input)) {
	case 0:
		return;
	case -1:
		/* disconnected */
		error = conn->input->stream_errno != 0 ?
			strerror(conn->input->stream_errno) : "EOF";
		auth_server_connection_reconnect(conn, error);
		return;
	case -2:
		/* buffer full - can't happen unless auth is buggy */
		i_error("BUG: Auth server sent us more than %d bytes of data",
			AUTH_SERVER_CONN_MAX_LINE_LENGTH);
		auth_server_connection_disconnect(conn, "buffer full");
		return;
	}

	if (!conn->version_received) {
		line = i_stream_next_line(conn->input);
		if (line == NULL)
			return;

		/* make sure the major version matches */
		if (strncmp(line, "VERSION\t", 8) != 0 ||
		    !str_uint_equals(t_strcut(line + 8, '\t'),
				     AUTH_CLIENT_PROTOCOL_MAJOR_VERSION)) {
			i_error("Authentication server not compatible with "
				"this client (mixed old and new binaries?)");
			auth_server_connection_disconnect(conn,
				"incompatible server");
			return;
		}
		conn->version_received = TRUE;
	}

	input = conn->input;
	i_stream_ref(input);
	while ((line = i_stream_next_line(input)) != NULL && !input->closed) {
		T_BEGIN {
			ret = auth_server_connection_input_line(conn, line);
		} T_END;

		if (ret < 0) {
			auth_server_connection_disconnect(conn, t_strdup_printf(
				"Received broken input: %s", line));
			break;
		}
	}
	i_stream_unref(&input);
}