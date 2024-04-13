g_socket_client_connect_to_service_finish (GSocketClient  *client,
					   GAsyncResult   *result,
					   GError        **error)
{
  return g_socket_client_connect_finish (client, result, error);
}