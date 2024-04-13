g_socket_client_set_timeout (GSocketClient *client,
			     guint          timeout)
{
  if (client->priv->timeout == timeout)
    return;

  client->priv->timeout = timeout;
  g_object_notify (G_OBJECT (client), "timeout");
}