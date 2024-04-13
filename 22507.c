g_socket_client_get_timeout (GSocketClient *client)
{
  return client->priv->timeout;
}