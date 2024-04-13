auth_server_lookup_request(struct auth_server_connection *conn,
			   const char *id_arg, bool remove,
			   struct auth_client_request **request_r)
{
	struct auth_client_request *request;
	unsigned int id;

	if (id_arg == NULL || str_to_uint(id_arg, &id) < 0) {
		i_error("BUG: Authentication server input missing ID");
		return -1;
	}

	request = hash_table_lookup(conn->requests, POINTER_CAST(id));
	if (request == NULL) {
		i_error("BUG: Authentication server sent unknown id %u", id);
		return -1;
	}
	if (remove || auth_client_request_is_aborted(request))
		hash_table_remove(conn->requests, POINTER_CAST(id));

	*request_r = request;
	return 0;
}