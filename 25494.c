static bool do_uninstall(THD *thd, TABLE *table, const LEX_STRING *name)
{
  struct st_plugin_int *plugin;
  mysql_mutex_assert_owner(&LOCK_plugin);

  if (!(plugin= plugin_find_internal(name, MYSQL_ANY_PLUGIN)) ||
      plugin->state & (PLUGIN_IS_UNINITIALIZED | PLUGIN_IS_DYING))
  {
    // maybe plugin is in mysql.plugin present so postpond the error
    plugin= NULL;
  }

  if (plugin)
  {
    if (!plugin->plugin_dl)
    {
      my_error(ER_PLUGIN_DELETE_BUILTIN, MYF(0));
      return 1;
    }
    if (plugin->load_option == PLUGIN_FORCE_PLUS_PERMANENT)
    {
      my_error(ER_PLUGIN_IS_PERMANENT, MYF(0), name->str);
      return 1;
    }

    plugin->state= PLUGIN_IS_DELETED;
    if (plugin->ref_count)
      push_warning(thd, Sql_condition::WARN_LEVEL_WARN,
          WARN_PLUGIN_BUSY, ER_THD(thd, WARN_PLUGIN_BUSY));
    else
      reap_needed= true;
  }

  uchar user_key[MAX_KEY_LENGTH];
  table->use_all_columns();
  table->field[0]->store(name->str, name->length, system_charset_info);
  key_copy(user_key, table->record[0], table->key_info,
           table->key_info->key_length);
  if (! table->file->ha_index_read_idx_map(table->record[0], 0, user_key,
                                           HA_WHOLE_KEY, HA_READ_KEY_EXACT))
  {
    int error;
    /*
      We do not replicate the UNINSTALL PLUGIN statement. Disable binlogging
      of the delete from the plugin table, so that it is not replicated in
      row based mode.
    */
    tmp_disable_binlog(thd);
    error= table->file->ha_delete_row(table->record[0]);
    reenable_binlog(thd);
    if (error)
    {
      table->file->print_error(error, MYF(0));
      return 1;
    }
  }
  else if (!plugin)
  {
    my_error(ER_SP_DOES_NOT_EXIST, MYF(0), "PLUGIN", name->str);
    return 1;
  }
  return 0;
}