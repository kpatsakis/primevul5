void wsrep_plugins_pre_init()
{
  wsrep_dummy_plugin_ptr= &wsrep_dummy_plugin;
  wsrep_dummy_plugin.state= PLUGIN_IS_DISABLED;
  global_system_variables.table_plugin=
    plugin_int_to_ref(wsrep_dummy_plugin_ptr);
}