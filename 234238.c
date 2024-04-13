static int auth_server_input_cont(struct auth_server_connection *conn,
				  const char *const *args)
{
	struct auth_client_request *request;

	if (str_array_length(args) < 2) {
		i_error("BUG: Authentication server sent broken CONT line");
		return -1;
	}

	if (auth_server_lookup_request(conn, args[0], FALSE, &request) < 0)
		return -1;
	auth_client_request_server_input(request, AUTH_REQUEST_STATUS_CONTINUE,
					 args + 1);
	return 0;
}