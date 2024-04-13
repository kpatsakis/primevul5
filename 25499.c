static int plugin_initialize(MEM_ROOT *tmp_root, struct st_plugin_int *plugin,
                             int *argc, char **argv, bool options_only)
{
  int ret= 1;
  DBUG_ENTER("plugin_initialize");

  mysql_mutex_assert_owner(&LOCK_plugin);
  uint state= plugin->state;
  DBUG_ASSERT(state == PLUGIN_IS_UNINITIALIZED);

  mysql_mutex_unlock(&LOCK_plugin);

  mysql_prlock_wrlock(&LOCK_system_variables_hash);
  if (test_plugin_options(tmp_root, plugin, argc, argv))
    state= PLUGIN_IS_DISABLED;
  mysql_prlock_unlock(&LOCK_system_variables_hash);

  if (options_only || state == PLUGIN_IS_DISABLED)
  {
    ret= !options_only && plugin_is_forced(plugin);
    state= PLUGIN_IS_DISABLED;
    goto err;
  }

  if (plugin_type_initialize[plugin->plugin->type])
  {
    if ((*plugin_type_initialize[plugin->plugin->type])(plugin))
    {
      sql_print_error("Plugin '%s' registration as a %s failed.",
                      plugin->name.str, plugin_type_names[plugin->plugin->type].str);
      goto err;
    }
  }
  else if (plugin->plugin->init)
  {
    if (plugin->plugin->init(plugin))
    {
      sql_print_error("Plugin '%s' init function returned error.",
                      plugin->name.str);
      goto err;
    }
  }
  state= PLUGIN_IS_READY; // plugin->init() succeeded

  if (plugin->plugin->status_vars)
  {
    /*
      historical ndb behavior caused MySQL plugins to specify
      status var names in full, with the plugin name prefix.
      this was never fixed in MySQL.
      MariaDB fixes that but supports MySQL style too.
    */
    SHOW_VAR *show_vars= plugin->plugin->status_vars;
    SHOW_VAR tmp_array[2]= {
      {plugin->plugin->name, (char*)plugin->plugin->status_vars, SHOW_ARRAY},
      {0, 0, SHOW_UNDEF}
    };
    if (strncasecmp(show_vars->name, plugin->name.str, plugin->name.length))
      show_vars= tmp_array;

    if (add_status_vars(show_vars))
      goto err;
  }

  ret= 0;

err:
  if (ret)
    plugin_variables_deinit(plugin);

  mysql_mutex_lock(&LOCK_plugin);
  plugin->state= state;

  DBUG_RETURN(ret);
}