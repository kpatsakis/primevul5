g_socket_client_connect_finish (GSocketClient  *client,
				GAsyncResult   *result,
				GError        **error)
{
  g_return_val_if_fail (g_task_is_valid (result, client), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}