static void auth_server_reconnect_timeout(struct auth_server_connection *conn)
{
	(void)auth_server_connection_connect(conn);
}