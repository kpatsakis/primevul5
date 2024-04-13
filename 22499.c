can_use_proxy (GSocketClient *client)
{
  GSocketClientPrivate *priv = client->priv;

  return priv->enable_proxy
          && priv->type == G_SOCKET_TYPE_STREAM;
}