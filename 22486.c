g_socket_client_finalize (GObject *object)
{
  GSocketClient *client = G_SOCKET_CLIENT (object);

  g_clear_object (&client->priv->local_address);
  g_clear_object (&client->priv->proxy_resolver);

  G_OBJECT_CLASS (g_socket_client_parent_class)->finalize (object);

  g_hash_table_unref (client->priv->app_proxies);
}