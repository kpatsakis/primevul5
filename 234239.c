void auth_server_connection_disconnect(struct auth_server_connection *conn,
				       const char *reason)
{
	if (!conn->connected)
		return;
	conn->connected = FALSE;
	conn->handshake_received = FALSE;
	conn->version_received = FALSE;
	conn->has_plain_mech = FALSE;
	conn->server_pid = 0;
	conn->connect_uid = 0;
	conn->cookie = NULL;
	array_clear(&conn->available_auth_mechs);

	timeout_remove(&conn->to);
	io_remove(&conn->io);
	if (conn->fd != -1) {
		i_stream_destroy(&conn->input);
		o_stream_destroy(&conn->output);

		if (close(conn->fd) < 0)
			i_error("close(auth server connection) failed: %m");
		conn->fd = -1;
	}

	auth_server_connection_remove_requests(conn, reason);

	if (conn->client->connect_notify_callback != NULL) {
		conn->client->connect_notify_callback(conn->client, FALSE,
				conn->client->connect_notify_context);
	}
}