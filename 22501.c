g_socket_client_get_socket_type (GSocketClient *client)
{
  return client->priv->type;
}