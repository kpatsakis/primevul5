sys_var *find_sys_var_ex(THD *thd, const char *str, size_t length,
                         bool throw_error, bool locked)
{
  sys_var *var;
  sys_var_pluginvar *pi= NULL;
  plugin_ref plugin;
  DBUG_ENTER("find_sys_var_ex");
  DBUG_PRINT("enter", ("var '%.*s'", (int)length, str));

  if (!locked)
    mysql_mutex_lock(&LOCK_plugin);
  mysql_prlock_rdlock(&LOCK_system_variables_hash);
  if ((var= intern_find_sys_var(str, length)) &&
      (pi= var->cast_pluginvar()))
  {
    mysql_prlock_unlock(&LOCK_system_variables_hash);
    LEX *lex= thd ? thd->lex : 0;
    if (!(plugin= intern_plugin_lock(lex, plugin_int_to_ref(pi->plugin))))
      var= NULL; /* failed to lock it, it must be uninstalling */
    else
    if (!(plugin_state(plugin) & PLUGIN_IS_READY))
    {
      /* initialization not completed */
      var= NULL;
      intern_plugin_unlock(lex, plugin);
    }
  }
  else
    mysql_prlock_unlock(&LOCK_system_variables_hash);
  if (!locked)
    mysql_mutex_unlock(&LOCK_plugin);

  if (!throw_error && !var)
    my_error(ER_UNKNOWN_SYSTEM_VARIABLE, MYF(0), (int)length, (char*) str);
  DBUG_RETURN(var);
}