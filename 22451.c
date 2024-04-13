create_socket (GSocketClient  *client,
	       GSocketAddress *dest_address,
	       GError        **error)
{
  GSocketFamily family;
  GSocket *socket;

  family = client->priv->family;
  if (family == G_SOCKET_FAMILY_INVALID &&
      client->priv->local_address != NULL)
    family = g_socket_address_get_family (client->priv->local_address);
  if (family == G_SOCKET_FAMILY_INVALID)
    family = g_socket_address_get_family (dest_address);

  socket = g_socket_new (family,
			 client->priv->type,
			 client->priv->protocol,
			 error);
  if (socket == NULL)
    return NULL;

  if (client->priv->local_address)
    {
      if (!g_socket_bind (socket,
			  client->priv->local_address,
			  FALSE,
			  error))
	{
	  g_object_unref (socket);
	  return NULL;
	}
    }

  if (client->priv->timeout)
    g_socket_set_timeout (socket, client->priv->timeout);

  return socket;
}