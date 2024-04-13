g_socket_client_get_family (GSocketClient *client)
{
  return client->priv->family;
}