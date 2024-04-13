static void init_plugin_psi_keys(void)
{
  const char* category= "sql";
  int count;

  if (PSI_server == NULL)
    return;

  count= array_elements(all_plugin_mutexes);
  PSI_server->register_mutex(category, all_plugin_mutexes, count);
}