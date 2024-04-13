auth_server_connection_remove_requests(struct auth_server_connection *conn,
				       const char *disconnect_reason)
{
	static const char *const temp_failure_args[] = { "temp", NULL };
	struct hash_iterate_context *iter;
	void *key;
	struct auth_client_request *request;
	time_t created, oldest = 0;
	unsigned int request_count = 0;

	if (hash_table_count(conn->requests) == 0)
		return;

	iter = hash_table_iterate_init(conn->requests);
	while (hash_table_iterate(iter, conn->requests, &key, &request)) {
		if (!auth_client_request_is_aborted(request)) {
			request_count++;
			created = auth_client_request_get_create_time(request);
			if (oldest > created || oldest == 0)
				oldest = created;
		}

		auth_client_request_server_input(request,
			AUTH_REQUEST_STATUS_INTERNAL_FAIL,
			temp_failure_args);
	}
	hash_table_iterate_deinit(&iter);
	hash_table_clear(conn->requests, FALSE);

	if (request_count > 0) {
		i_warning("Auth connection closed with %u pending requests "
			  "(max %u secs, pid=%s, %s)", request_count,
			  (unsigned int)(ioloop_time - oldest),
			  my_pid, disconnect_reason);
	}
}