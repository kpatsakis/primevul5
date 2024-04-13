void STDCALL mysql_server_end(void)
{
  if (!mysql_client_init)
    return;

  release_configuration_dirs();
  mysql_client_plugin_deinit();

  list_free(pvio_callback, 0);
  if (ma_init_done)
    ma_end(0);
#ifdef HAVE_TLS
  ma_pvio_tls_end();
#endif
  mysql_client_init= 0;
  ma_init_done= 0;
#ifdef WIN32
  init_once = (INIT_ONCE)INIT_ONCE_STATIC_INIT;
#else
  init_once = (pthread_once_t)PTHREAD_ONCE_INIT;
#endif
}