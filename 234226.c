auth_server_input_cookie(struct auth_server_connection *conn,
			 const char *const *args)
{
	if (conn->cookie != NULL) {
		i_error("BUG: Authentication server already sent cookie");
		return -1;
	}
	conn->cookie = p_strdup(conn->pool, args[0]);
	return 0;
}