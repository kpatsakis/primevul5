g_socket_client_get_tls (GSocketClient *client)
{
  return client->priv->tls;
}