void plugin_thdvar_init(THD *thd)
{
  plugin_ref old_table_plugin= thd->variables.table_plugin;
  plugin_ref old_tmp_table_plugin= thd->variables.tmp_table_plugin;
  plugin_ref old_enforced_table_plugin= thd->variables.enforced_table_plugin;
  DBUG_ENTER("plugin_thdvar_init");

  // This function may be called many times per THD (e.g. on COM_CHANGE_USER)
  thd->variables.table_plugin= NULL;
  thd->variables.tmp_table_plugin= NULL;
  thd->variables.enforced_table_plugin= NULL;
  cleanup_variables(&thd->variables);

  thd->variables= global_system_variables;

  /* we are going to allocate these lazily */
  thd->variables.dynamic_variables_version= 0;
  thd->variables.dynamic_variables_size= 0;
  thd->variables.dynamic_variables_ptr= 0;

  mysql_mutex_lock(&LOCK_plugin);
  thd->variables.table_plugin=
      intern_plugin_lock(NULL, global_system_variables.table_plugin);
  if (global_system_variables.tmp_table_plugin)
    thd->variables.tmp_table_plugin=
          intern_plugin_lock(NULL, global_system_variables.tmp_table_plugin);
  if (global_system_variables.enforced_table_plugin)
    thd->variables.enforced_table_plugin=
          intern_plugin_lock(NULL, global_system_variables.enforced_table_plugin);
  intern_plugin_unlock(NULL, old_table_plugin);
  intern_plugin_unlock(NULL, old_tmp_table_plugin);
  intern_plugin_unlock(NULL, old_enforced_table_plugin);
  mysql_mutex_unlock(&LOCK_plugin);

  DBUG_VOID_RETURN;
}