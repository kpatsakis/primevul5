g_socket_client_get_local_address (GSocketClient *client)
{
  return client->priv->local_address;
}