static bool finalize_install(THD *thd, TABLE *table, const LEX_STRING *name,
                             int *argc, char **argv)
{
  struct st_plugin_int *tmp= plugin_find_internal(name, MYSQL_ANY_PLUGIN);
  int error;
  DBUG_ASSERT(tmp);
  mysql_mutex_assert_owner(&LOCK_plugin); // because of tmp->state

  if (tmp->state != PLUGIN_IS_UNINITIALIZED)
  {
    /* already installed */
    return 0;
  }
  else
  {
    if (plugin_initialize(thd->mem_root, tmp, argc, argv, false))
    {
      report_error(REPORT_TO_USER, ER_CANT_INITIALIZE_UDF, name->str,
                   "Plugin initialization function failed.");
      tmp->state= PLUGIN_IS_DELETED;
      return 1;
    }
  }
  if (tmp->state == PLUGIN_IS_DISABLED)
  {
    if (global_system_variables.log_warnings)
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                          ER_CANT_INITIALIZE_UDF,
                          ER_THD(thd, ER_CANT_INITIALIZE_UDF),
                          name->str, "Plugin is disabled");
  }

  /*
    We do not replicate the INSTALL PLUGIN statement. Disable binlogging
    of the insert into the plugin table, so that it is not replicated in
    row based mode.
  */
  tmp_disable_binlog(thd);
  table->use_all_columns();
  restore_record(table, s->default_values);
  table->field[0]->store(name->str, name->length, system_charset_info);
  table->field[1]->store(tmp->plugin_dl->dl.str, tmp->plugin_dl->dl.length,
                         files_charset_info);
  error= table->file->ha_write_row(table->record[0]);
  reenable_binlog(thd);
  if (error)
  {
    table->file->print_error(error, MYF(0));
    tmp->state= PLUGIN_IS_DELETED;
    return 1;
  }
  return 0;
}