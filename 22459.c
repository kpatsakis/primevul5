g_socket_client_set_enable_proxy (GSocketClient *client,
				  gboolean       enable)
{
  enable = !!enable;
  if (client->priv->enable_proxy == enable)
    return;

  client->priv->enable_proxy = enable;
  g_object_notify (G_OBJECT (client), "enable-proxy");
}