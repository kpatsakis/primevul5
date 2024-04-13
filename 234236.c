static int auth_server_input_fail(struct auth_server_connection *conn,
				  const char *const *args)
{
	struct auth_client_request *request;

	if (auth_server_lookup_request(conn, args[0], TRUE, &request) < 0)
		return -1;
	auth_client_request_server_input(request, AUTH_REQUEST_STATUS_FAIL,
					 args + 1);
	return 0;
}