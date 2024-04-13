g_socket_client_get_proxy_resolver (GSocketClient *client)
{
  if (client->priv->proxy_resolver)
    return client->priv->proxy_resolver;
  else
    return g_proxy_resolver_get_default ();
}