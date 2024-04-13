static bool plugin_add(MEM_ROOT *tmp_root,
                       const LEX_STRING *name, LEX_STRING *dl, int report)
{
  struct st_plugin_int tmp, *maybe_dupe;
  struct st_maria_plugin *plugin;
  uint oks= 0, errs= 0, dupes= 0;
  DBUG_ENTER("plugin_add");
  DBUG_PRINT("enter", ("name: %s  dl: %s", name->str, dl->str));

  if (name->str && plugin_find_internal(name, MYSQL_ANY_PLUGIN))
  {
    report_error(report, ER_PLUGIN_INSTALLED, name->str);
    DBUG_RETURN(TRUE);
  }
  /* Clear the whole struct to catch future extensions. */
  bzero((char*) &tmp, sizeof(tmp));
  fix_dl_name(tmp_root, dl);
  if (! (tmp.plugin_dl= plugin_dl_add(dl, report)))
    DBUG_RETURN(TRUE);
  /* Find plugin by name */
  for (plugin= tmp.plugin_dl->plugins; plugin->info; plugin++)
  {
    tmp.name.str= (char *)plugin->name;
    tmp.name.length= strlen(plugin->name);

    if (plugin->type < 0 || plugin->type >= MYSQL_MAX_PLUGIN_TYPE_NUM)
      continue; // invalid plugin type

    if (plugin->type == MYSQL_UDF_PLUGIN ||
        (plugin->type == MariaDB_PASSWORD_VALIDATION_PLUGIN &&
         tmp.plugin_dl->mariaversion == 0))
      continue; // unsupported plugin type

    if (name->str && my_strnncoll(system_charset_info,
                                  (const uchar *)name->str, name->length,
                                  (const uchar *)tmp.name.str, tmp.name.length))
      continue; // plugin name doesn't match

    if (!name->str &&
        (maybe_dupe= plugin_find_internal(&tmp.name, MYSQL_ANY_PLUGIN)))
    {
      if (plugin->name != maybe_dupe->plugin->name)
      {
        report_error(report, ER_UDF_EXISTS, plugin->name);
        DBUG_RETURN(TRUE);
      }
      dupes++;
      continue; // already installed
    }
    struct st_plugin_int *tmp_plugin_ptr;
    if (*(int*)plugin->info <
        min_plugin_info_interface_version[plugin->type] ||
        ((*(int*)plugin->info) >> 8) >
        (cur_plugin_info_interface_version[plugin->type] >> 8))
    {
      char buf[256];
      strxnmov(buf, sizeof(buf) - 1, "API version for ",
               plugin_type_names[plugin->type].str,
               " plugin ", tmp.name.str,
               " not supported by this version of the server", NullS);
      report_error(report, ER_CANT_OPEN_LIBRARY, dl->str, ENOEXEC, buf);
      goto err;
    }
    if (plugin_maturity_map[plugin->maturity] < plugin_maturity)
    {
      char buf[256];
      strxnmov(buf, sizeof(buf) - 1, "Loading of ",
               plugin_maturity_names[plugin->maturity],
               " plugin ", tmp.name.str,
               " is prohibited by --plugin-maturity=",
               plugin_maturity_names[plugin_maturity],
               NullS);
      report_error(report, ER_CANT_OPEN_LIBRARY, dl->str, EPERM, buf);
      goto err;
    }
    tmp.plugin= plugin;
    tmp.ref_count= 0;
    tmp.state= PLUGIN_IS_UNINITIALIZED;
    tmp.load_option= PLUGIN_ON;

    if (!(tmp_plugin_ptr= plugin_insert_or_reuse(&tmp)))
      goto err;
    if (my_hash_insert(&plugin_hash[plugin->type], (uchar*)tmp_plugin_ptr))
      tmp_plugin_ptr->state= PLUGIN_IS_FREED;
    init_alloc_root(&tmp_plugin_ptr->mem_root, 4096, 4096, MYF(0));

    if (name->str)
      DBUG_RETURN(FALSE); // all done

    oks++;
    tmp.plugin_dl->ref_count++;
    continue; // otherwise - go on

err:
    errs++;
    if (name->str)
      break;
  }

  DBUG_ASSERT(!name->str || !dupes); // dupes is ONLY for name->str == 0

  if (errs == 0 && oks == 0 && !dupes) // no plugin was found
    report_error(report, ER_CANT_FIND_DL_ENTRY, name->str);

  plugin_dl_del(tmp.plugin_dl);
  DBUG_RETURN(errs > 0 || oks + dupes == 0);
}