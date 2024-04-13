g_socket_client_set_local_address (GSocketClient  *client,
				   GSocketAddress *address)
{
  if (address)
    g_object_ref (address);

  if (client->priv->local_address)
    {
      g_object_unref (client->priv->local_address);
    }
  client->priv->local_address = address;
  g_object_notify (G_OBJECT (client), "local-address");
}