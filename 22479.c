g_socket_client_set_proxy_resolver (GSocketClient  *client,
                                    GProxyResolver *proxy_resolver)
{
  /* We have to be careful to avoid calling
   * g_proxy_resolver_get_default() until we're sure we need it,
   * because trying to load the default proxy resolver module will
   * break some test programs that aren't expecting it (eg,
   * tests/gsettings).
   */

  if (client->priv->proxy_resolver)
    g_object_unref (client->priv->proxy_resolver);

  client->priv->proxy_resolver = proxy_resolver;

  if (client->priv->proxy_resolver)
    g_object_ref (client->priv->proxy_resolver);
}