g_socket_client_init (GSocketClient *client)
{
  client->priv = g_socket_client_get_instance_private (client);
  client->priv->type = G_SOCKET_TYPE_STREAM;
  client->priv->app_proxies = g_hash_table_new_full (g_str_hash,
						     g_str_equal,
						     g_free,
						     NULL);
}