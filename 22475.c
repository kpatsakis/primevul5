g_socket_client_connect_to_uri (GSocketClient  *client,
				const gchar    *uri,
				guint16         default_port,
				GCancellable   *cancellable,
				GError        **error)
{
  GSocketConnectable *connectable;
  GSocketConnection *connection;

  connectable = g_network_address_parse_uri (uri, default_port, error);
  if (connectable == NULL)
    return NULL;

  connection = g_socket_client_connect (client, connectable,
					cancellable, error);
  g_object_unref (connectable);

  return connection;
}