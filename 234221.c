int auth_server_connection_connect(struct auth_server_connection *conn)
{
	const char *handshake;
	int fd;

	i_assert(!conn->connected);
	i_assert(conn->fd == -1);

	conn->last_connect = ioloop_time;
	timeout_remove(&conn->to);

	/* max. 1 second wait here. */
	fd = net_connect_unix_with_retries(conn->client->auth_socket_path,
					   1000);
	if (fd == -1) {
		if (errno == EACCES) {
			i_error("auth: %s",
				eacces_error_get("connect",
					conn->client->auth_socket_path));
		} else {
			i_error("auth: connect(%s) failed: %m",
				conn->client->auth_socket_path);
		}
		return -1;
	}
	conn->fd = fd;
	conn->io = io_add(fd, IO_READ, auth_server_connection_input, conn);
	conn->input = i_stream_create_fd(fd, AUTH_SERVER_CONN_MAX_LINE_LENGTH);
	conn->output = o_stream_create_fd(fd, (size_t)-1);
	conn->connected = TRUE;

	handshake = t_strdup_printf("VERSION\t%u\t%u\nCPID\t%u\n",
				    AUTH_CLIENT_PROTOCOL_MAJOR_VERSION,
                                    AUTH_CLIENT_PROTOCOL_MINOR_VERSION,
				    conn->client->client_pid);
	if (o_stream_send_str(conn->output, handshake) < 0) {
		i_warning("Error sending handshake to auth server: %s",
			  o_stream_get_error(conn->output));
		auth_server_connection_disconnect(conn,
			o_stream_get_error(conn->output));
		return -1;
	}

	conn->to = timeout_add(AUTH_HANDSHAKE_TIMEOUT,
			       auth_client_handshake_timeout, conn);
	return 0;
}