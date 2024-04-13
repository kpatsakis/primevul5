void auth_server_connection_deinit(struct auth_server_connection **_conn)
{
        struct auth_server_connection *conn = *_conn;

	*_conn = NULL;

	auth_server_connection_disconnect(conn, "deinitializing");
	i_assert(hash_table_count(conn->requests) == 0);
	hash_table_destroy(&conn->requests);
	array_free(&conn->available_auth_mechs);
	pool_unref(&conn->pool);
}