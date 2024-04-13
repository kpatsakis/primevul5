static void plugin_variables_deinit(struct st_plugin_int *plugin)
{

  for (sys_var *var= plugin->system_vars; var; var= var->next)
    (*var->test_load)= FALSE;
  mysql_del_sys_var_chain(plugin->system_vars);
}