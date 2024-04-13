g_socket_client_connect_to_uri_async (GSocketClient        *client,
				      const gchar          *uri,
				      guint16               default_port,
				      GCancellable         *cancellable,
				      GAsyncReadyCallback   callback,
				      gpointer              user_data)
{
  GSocketConnectable *connectable;
  GError *error;

  error = NULL;
  connectable = g_network_address_parse_uri (uri, default_port, &error);
  if (connectable == NULL)
    {
      g_task_report_error (client, callback, user_data,
                           g_socket_client_connect_to_uri_async,
                           error);
    }
  else
    {
      g_socket_client_connect_async (client,
				     connectable, cancellable,
				     callback, user_data);
      g_object_unref (connectable);
    }
}