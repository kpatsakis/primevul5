g_socket_client_connect_to_host_async (GSocketClient        *client,
				       const gchar          *host_and_port,
				       guint16               default_port,
				       GCancellable         *cancellable,
				       GAsyncReadyCallback   callback,
				       gpointer              user_data)
{
  GSocketConnectable *connectable;
  GError *error;

  error = NULL;
  connectable = g_network_address_parse (host_and_port, default_port,
					 &error);
  if (connectable == NULL)
    {
      g_task_report_error (client, callback, user_data,
                           g_socket_client_connect_to_host_async,
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