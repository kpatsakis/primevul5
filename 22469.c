g_socket_client_add_application_proxy (GSocketClient *client,
			               const gchar   *protocol)
{
  g_hash_table_add (client->priv->app_proxies, g_strdup (protocol));
}