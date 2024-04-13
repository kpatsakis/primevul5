auth_server_input_spid(struct auth_server_connection *conn,
		       const char *const *args)
{
	if (conn->handshake_received) {
		i_error("BUG: Authentication server already sent handshake");
		return -1;
	}

	if (str_to_uint(args[0], &conn->server_pid) < 0) {
		i_error("BUG: Authentication server sent invalid PID");
		return -1;
	}
	return 0;
}