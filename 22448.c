on_connected_cancelled (GObject      *source_object,
                        GAsyncResult *result,
                        gpointer      user_data)
{
  GSocketConnection *conn;
  GError *error = NULL;

  conn = g_socket_client_connect_to_uri_finish (G_SOCKET_CLIENT (source_object), result, &error);
  g_assert_error (error, G_IO_ERROR, G_IO_ERROR_CANCELLED);
  g_assert_null (conn);

  g_error_free (error);
  g_main_loop_quit (user_data);
}