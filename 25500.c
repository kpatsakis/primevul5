bool mysql_uninstall_plugin(THD *thd, const LEX_STRING *name,
                            const LEX_STRING *dl_arg)
{
  TABLE *table;
  TABLE_LIST tables;
  LEX_STRING dl= *dl_arg;
  bool error= false;
  unsigned long event_class_mask[MYSQL_AUDIT_CLASS_MASK_SIZE] =
  { MYSQL_AUDIT_GENERAL_CLASSMASK };
  DBUG_ENTER("mysql_uninstall_plugin");

  tables.init_one_table("mysql", 5, "plugin", 6, "plugin", TL_WRITE);

  if (!opt_noacl && check_table_access(thd, DELETE_ACL, &tables, FALSE, 1, FALSE))
    DBUG_RETURN(TRUE);

  WSREP_TO_ISOLATION_BEGIN(WSREP_MYSQL_DB, NULL, NULL)

  /* need to open before acquiring LOCK_plugin or it will deadlock */
  if (! (table= open_ltable(thd, &tables, TL_WRITE, MYSQL_LOCK_IGNORE_TIMEOUT)))
    DBUG_RETURN(TRUE);

  if (!table->key_info)
  {
    my_printf_error(ER_UNKNOWN_ERROR,
                    "The table %s.%s has no primary key. "
                    "Please check the table definition and "
                    "create the primary key accordingly.", MYF(0),
                    table->s->db.str, table->s->table_name.str);
    DBUG_RETURN(TRUE);
  }

  /*
    Pre-acquire audit plugins for events that may potentially occur
    during [UN]INSTALL PLUGIN.

    When audit event is triggered, audit subsystem acquires interested
    plugins by walking through plugin list. Evidently plugin list
    iterator protects plugin list by acquiring LOCK_plugin, see
    plugin_foreach_with_mask().

    On the other hand [UN]INSTALL PLUGIN is acquiring LOCK_plugin
    rather for a long time.

    When audit event is triggered during [UN]INSTALL PLUGIN, plugin
    list iterator acquires the same lock (within the same thread)
    second time.

    This hack should be removed when LOCK_plugin is fixed so it
    protects only what it supposed to protect.

    See also mysql_install_plugin() and initialize_audit_plugin()
  */
  if (mysql_audit_general_enabled())
    mysql_audit_acquire_plugins(thd, event_class_mask);

  mysql_mutex_lock(&LOCK_plugin);

  if (name->str)
    error= do_uninstall(thd, table, name);
  else
  {
    fix_dl_name(thd->mem_root, &dl);
    st_plugin_dl *plugin_dl= plugin_dl_find(&dl);
    if (plugin_dl)
    {
      for (struct st_maria_plugin *plugin= plugin_dl->plugins;
           plugin->info; plugin++)
      {
        LEX_STRING str= { const_cast<char*>(plugin->name), strlen(plugin->name) };
        error|= do_uninstall(thd, table, &str);
      }
    }
    else
    {
      my_error(ER_SP_DOES_NOT_EXIST, MYF(0), "SONAME", dl.str);
      error= true;
    }
  }
  reap_plugins();

  global_plugin_version++;
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_RETURN(error);

WSREP_ERROR_LABEL:
  DBUG_RETURN(TRUE);
}