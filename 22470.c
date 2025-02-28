g_socket_client_connect_async (GSocketClient       *client,
			       GSocketConnectable  *connectable,
			       GCancellable        *cancellable,
			       GAsyncReadyCallback  callback,
			       gpointer             user_data)
{
  GSocketClientAsyncConnectData *data;

  g_return_if_fail (G_IS_SOCKET_CLIENT (client));

  data = g_slice_new0 (GSocketClientAsyncConnectData);
  data->client = client;
  data->connectable = g_object_ref (connectable);

  if (can_use_proxy (client))
    {
      data->enumerator = g_socket_connectable_proxy_enumerate (connectable);
      if (client->priv->proxy_resolver &&
          G_IS_PROXY_ADDRESS_ENUMERATOR (data->enumerator))
        {
          g_object_set (G_OBJECT (data->enumerator),
                        "proxy-resolver", client->priv->proxy_resolver,
                        NULL);
        }
    }
  else
    data->enumerator = g_socket_connectable_enumerate (connectable);

  /* The flow and ownership here isn't quite obvious:
    - The task starts an async attempt to connect.
      - Each attempt holds a single ref on task.
      - Each attempt may create new attempts by timing out (not a failure) so
        there are multiple attempts happening in parallel.
      - Upon failure an attempt will start a new attempt that steals its ref
        until there are no more attempts left and it drops its ref.
      - Upon success it will cancel all other attempts and continue on
        to the rest of the connection (tls, proxies, etc) which do not
        happen in parallel and at the very end drop its ref.
      - Upon cancellation an attempt drops its ref.
   */

  data->task = g_task_new (client, cancellable, callback, user_data);
  g_task_set_source_tag (data->task, g_socket_client_connect_async);
  g_task_set_task_data (data->task, data, (GDestroyNotify)g_socket_client_async_connect_data_free);

  enumerator_next_async (data, FALSE);
}