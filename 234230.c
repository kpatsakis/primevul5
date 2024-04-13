auth_server_input_cuid(struct auth_server_connection *conn,
		       const char *const *args)
{
	if (conn->handshake_received) {
		i_error("BUG: Authentication server already sent handshake");
		return -1;
	}
	if (args[0] == NULL ||
	    str_to_uint(args[0], &conn->connect_uid) < 0) {
		i_error("BUG: Authentication server sent broken CUID line");
		return -1;
	}
	return 0;
}