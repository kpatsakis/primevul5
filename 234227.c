void auth_server_connection_remove_request(struct auth_server_connection *conn,
					   unsigned int id)
{
	i_assert(conn->handshake_received);
	hash_table_remove(conn->requests, POINTER_CAST(id));
}