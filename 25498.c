void plugin_mutex_init()
{
#ifdef HAVE_PSI_INTERFACE
  init_plugin_psi_keys();
#endif
  mysql_mutex_init(key_LOCK_plugin, &LOCK_plugin, MY_MUTEX_INIT_FAST);
}