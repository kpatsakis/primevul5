g_socket_client_async_connect_complete (GSocketClientAsyncConnectData *data)
{
  g_assert (data->connection);

  if (!G_IS_SOCKET_CONNECTION (data->connection))
    {
      GSocketConnection *wrapper_connection;

      wrapper_connection = g_tcp_wrapper_connection_new (data->connection, data->socket);
      g_object_unref (data->connection);
      data->connection = (GIOStream *)wrapper_connection;
    }

  g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_COMPLETE, data->connectable, data->connection);
  g_task_return_pointer (data->task, data->connection, g_object_unref);
  data->connection = NULL;
  g_object_unref (data->task);
}