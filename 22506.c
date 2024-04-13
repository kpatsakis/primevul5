enumerator_next_async (GSocketClientAsyncConnectData *data,
                       gboolean                       add_task_ref)
{
  /* We need to cleanup the state */
  g_clear_object (&data->socket);
  g_clear_object (&data->proxy_addr);
  g_clear_object (&data->connection);

  /* Each enumeration takes a ref. This arg just avoids repeated unrefs when
     an enumeration starts another enumeration */
  if (add_task_ref)
    g_object_ref (data->task);

  g_socket_client_emit_event (data->client, G_SOCKET_CLIENT_RESOLVING, data->connectable, NULL);
  g_socket_address_enumerator_next_async (data->enumerator,
					  g_task_get_cancellable (data->task),
					  g_socket_client_enumerator_callback,
					  data);
}