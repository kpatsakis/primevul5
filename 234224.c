static void auth_client_handshake_timeout(struct auth_server_connection *conn)
{
	i_error("Timeout waiting for handshake from auth server. "
		"my pid=%u, input bytes=%"PRIuUOFF_T,
		conn->client->client_pid, conn->input->v_offset);
	auth_server_connection_reconnect(conn, "auth server timeout");
}