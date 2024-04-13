set_last_error (GSocketClientAsyncConnectData *data,
		GError *error)
{
  g_clear_error (&data->last_error);
  data->last_error = error;
}