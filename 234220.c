auth_server_connection_init(struct auth_client *client)
{
	struct auth_server_connection *conn;
	pool_t pool;

	pool = pool_alloconly_create("auth server connection", 1024);
	conn = p_new(pool, struct auth_server_connection, 1);
	conn->pool = pool;

	conn->client = client;
	conn->fd = -1;
	hash_table_create_direct(&conn->requests, pool, 100);
	i_array_init(&conn->available_auth_mechs, 8);
	return conn;
}