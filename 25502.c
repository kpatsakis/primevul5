sys_var *find_plugin_sysvar(st_plugin_int *plugin, st_mysql_sys_var *plugin_var)
{
  for (sys_var *var= plugin->system_vars; var; var= var->next)
  {
    sys_var_pluginvar *pvar=var->cast_pluginvar();
    if (pvar->plugin_var == plugin_var)
      return var;
  }
  return 0;
}