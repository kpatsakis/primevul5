g_socket_client_get_tls_validation_flags (GSocketClient *client)
{
  return client->priv->tls_validation_flags;
}