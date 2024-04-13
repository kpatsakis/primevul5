static my_bool read_mysql_plugin_info(struct st_plugin_dl *plugin_dl,
                                      void *sym, char *dlpath,
                                      int report)
{
  DBUG_ENTER("read_maria_plugin_info");
  /* Determine interface version */
  if (!sym)
  {
    report_error(report, ER_CANT_FIND_DL_ENTRY, plugin_interface_version_sym);
    DBUG_RETURN(TRUE);
  }
  plugin_dl->mariaversion= 0;
  plugin_dl->mysqlversion= *(int *)sym;
  /* Versioning */
  if (plugin_dl->mysqlversion < min_plugin_interface_version ||
      (plugin_dl->mysqlversion >> 8) > (MYSQL_PLUGIN_INTERFACE_VERSION >> 8))
  {
    report_error(report, ER_CANT_OPEN_LIBRARY, dlpath, ENOEXEC,
                 "plugin interface version mismatch");
    DBUG_RETURN(TRUE);
  }
  /* Find plugin declarations */
  if (!(sym= dlsym(plugin_dl->handle, plugin_declarations_sym)))
  {
    report_error(report, ER_CANT_FIND_DL_ENTRY, plugin_declarations_sym);
    DBUG_RETURN(TRUE);
  }

  /* convert mysql declaration to maria one */
  {
    int i;
    uint sizeof_st_plugin;
    struct st_mysql_plugin *old;
    struct st_maria_plugin *cur;
    char *ptr= (char *)sym;

    if ((sym= dlsym(plugin_dl->handle, sizeof_st_plugin_sym)))
      sizeof_st_plugin= *(int *)sym;
    else
    {
      DBUG_ASSERT(min_plugin_interface_version == 0);
      sizeof_st_plugin= (int)offsetof(struct st_mysql_plugin, version);
    }

    for (i= 0;
         ((struct st_mysql_plugin *)(ptr + i * sizeof_st_plugin))->info;
         i++)
      /* no op */;

    cur= (struct st_maria_plugin*)
          my_malloc((i + 1) * sizeof(struct st_maria_plugin),
                    MYF(MY_ZEROFILL|MY_WME));
    if (!cur)
    {
      report_error(report, ER_OUTOFMEMORY,
                   static_cast<int>(plugin_dl->dl.length));
      DBUG_RETURN(TRUE);
    }
    /*
      All st_plugin fields not initialized in the plugin explicitly, are
      set to 0. It matches C standard behaviour for struct initializers that
      have less values than the struct definition.
    */
    for (i=0;
         (old= (struct st_mysql_plugin *)(ptr + i * sizeof_st_plugin))->info;
         i++)
    {

      cur[i].type= old->type;
      cur[i].info= old->info;
      cur[i].name= old->name;
      cur[i].author= old->author;
      cur[i].descr= old->descr;
      cur[i].license= old->license;
      cur[i].init= old->init;
      cur[i].deinit= old->deinit;
      cur[i].version= old->version;
      cur[i].status_vars= old->status_vars;
      cur[i].system_vars= old->system_vars;
      /*
        Something like this should be added to process
        new mysql plugin versions:
        if (plugin_dl->mysqlversion > 0x0101)
        {
           cur[i].newfield= CONSTANT_MEANS_UNKNOWN;
        }
        else
        {
           cur[i].newfield= old->newfield;
        }
      */
      /* Maria only fields */
      cur[i].version_info= "Unknown";
      cur[i].maturity= MariaDB_PLUGIN_MATURITY_UNKNOWN;
    }
    plugin_dl->allocated= true;
    plugin_dl->plugins= (struct st_maria_plugin *)cur;
  }

  DBUG_RETURN(FALSE);
}