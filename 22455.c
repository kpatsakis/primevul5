test_happy_eyeballs_cancel (void)
{
  GSocketClient *client;
  GSocketService *service;
  GError *error = NULL;
  guint16 port;
  GMainLoop *loop;
  GCancellable *cancel;

  loop = g_main_loop_new (NULL, FALSE);

  service = g_socket_service_new ();
  port = g_socket_listener_add_any_inet_port (G_SOCKET_LISTENER (service), NULL, &error);
  g_assert_no_error (error);
  g_socket_service_start (service);

  client = g_socket_client_new ();
  cancel = g_cancellable_new ();
  g_socket_client_connect_to_host_async (client, "localhost", port, cancel, on_connected_cancelled, loop);
  g_timeout_add (1, (GSourceFunc) on_timer, cancel);
  g_main_loop_run (loop);

  g_main_loop_unref (loop);
  g_object_unref (service);
  g_object_unref (client);
  g_object_unref (cancel);
}