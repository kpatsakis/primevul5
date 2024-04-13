g_socket_client_get_enable_proxy (GSocketClient *client)
{
  return client->priv->enable_proxy;
}