auth_server_connection_input_line(struct auth_server_connection *conn,
				  const char *line)
{
	const char *const *args;

	if (conn->client->debug)
		i_debug("auth input: %s", line);

	args = t_strsplit_tabescaped(line);
	if (args[0] == NULL) {
		i_error("Auth server sent empty line");
		return -1;
	}
	if (strcmp(args[0], "OK") == 0)
		return auth_server_input_ok(conn, args + 1);
	else if (strcmp(args[0], "CONT") == 0)
		return auth_server_input_cont(conn, args + 1);
	else if (strcmp(args[0], "FAIL") == 0)
		return auth_server_input_fail(conn, args + 1);
	else if (strcmp(args[0], "MECH") == 0)
		return auth_server_input_mech(conn, args + 1);
	else if (strcmp(args[0], "SPID") == 0)
		return auth_server_input_spid(conn, args + 1);
	else if (strcmp(args[0], "CUID") == 0)
		return auth_server_input_cuid(conn, args + 1);
	else if (strcmp(args[0], "COOKIE") == 0)
		return auth_server_input_cookie(conn, args + 1);
	else if (strcmp(args[0], "DONE") == 0)
		return auth_server_input_done(conn);
	else {
		i_error("Auth server sent unknown command: %s", args[0]);
		return -1;
	}
}