static int auth_server_input_done(struct auth_server_connection *conn)
{
	if (array_count(&conn->available_auth_mechs) == 0) {
		i_error("BUG: Authentication server returned no mechanisms");
		return -1;
	}
	if (conn->cookie == NULL) {
		i_error("BUG: Authentication server didn't send a cookie");
		return -1;
	}

	timeout_remove(&conn->to);

	conn->handshake_received = TRUE;
	if (conn->client->connect_notify_callback != NULL) {
		conn->client->connect_notify_callback(conn->client, TRUE,
				conn->client->connect_notify_context);
	}
	return 0;
}