bool sys_var_pluginvar::global_update(THD *thd, set_var *var)
{
  DBUG_ASSERT(!is_readonly());
  mysql_mutex_assert_owner(&LOCK_global_system_variables);

  void *tgt= real_value_ptr(thd, OPT_GLOBAL);
  const void *src= &var->save_result;

  if (!var->value)
    src= var_def_ptr(plugin_var);

  plugin_var->update(thd, plugin_var, tgt, src);
  return false;
}