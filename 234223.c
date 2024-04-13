auth_server_connection_add_request(struct auth_server_connection *conn,
				   struct auth_client_request *request)
{
	unsigned int id;

	i_assert(conn->handshake_received);

	id = ++conn->client->request_id_counter;
	if (id == 0) {
		/* wrapped - ID 0 not allowed */
		id = ++conn->client->request_id_counter;
	}
	i_assert(hash_table_lookup(conn->requests, POINTER_CAST(id)) == NULL);
	hash_table_insert(conn->requests, POINTER_CAST(id), request);
	return id;
}