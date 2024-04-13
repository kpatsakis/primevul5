g_socket_client_set_tls (GSocketClient *client,
			 gboolean       tls)
{
  tls = !!tls;
  if (tls == client->priv->tls)
    return;

  client->priv->tls = tls;
  g_object_notify (G_OBJECT (client), "tls");
}