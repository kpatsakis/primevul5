auth_server_connection_reconnect(struct auth_server_connection *conn,
				 const char *disconnect_reason)
{
	time_t next_connect;

	auth_server_connection_disconnect(conn, disconnect_reason);

	next_connect = conn->last_connect + AUTH_SERVER_RECONNECT_TIMEOUT_SECS;
	conn->to = timeout_add(ioloop_time >= next_connect ? 0 :
			       (next_connect - ioloop_time) * 1000,
			       auth_server_reconnect_timeout, conn);
}