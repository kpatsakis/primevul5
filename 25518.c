static my_bool read_maria_plugin_info(struct st_plugin_dl *plugin_dl,
                                      void *sym, char *dlpath,
                                      int report)
{
  DBUG_ENTER("read_maria_plugin_info");

  /* Determine interface version */
  if (!(sym))
  {
    /*
      Actually this branch impossible because in case of absence of maria
      version we try mysql version.
    */
    report_error(report, ER_CANT_FIND_DL_ENTRY,
                 maria_plugin_interface_version_sym);
    DBUG_RETURN(TRUE);
  }
  plugin_dl->mariaversion= *(int *)sym;
  plugin_dl->mysqlversion= 0;
  /* Versioning */
  if (plugin_dl->mariaversion < min_maria_plugin_interface_version ||
      (plugin_dl->mariaversion >> 8) > (MARIA_PLUGIN_INTERFACE_VERSION >> 8))
  {
    report_error(report, ER_CANT_OPEN_LIBRARY, dlpath, ENOEXEC,
                 "plugin interface version mismatch");
    DBUG_RETURN(TRUE);
  }
  /* Find plugin declarations */
  if (!(sym= dlsym(plugin_dl->handle, maria_plugin_declarations_sym)))
  {
    report_error(report, ER_CANT_FIND_DL_ENTRY, maria_plugin_declarations_sym);
    DBUG_RETURN(TRUE);
  }
  if (plugin_dl->mariaversion != MARIA_PLUGIN_INTERFACE_VERSION)
  {
    uint sizeof_st_plugin;
    struct st_maria_plugin *old, *cur;
    char *ptr= (char *)sym;

    if ((sym= dlsym(plugin_dl->handle, maria_sizeof_st_plugin_sym)))
      sizeof_st_plugin= *(int *)sym;
    else
    {
      report_error(report, ER_CANT_FIND_DL_ENTRY, maria_sizeof_st_plugin_sym);
      DBUG_RETURN(TRUE);
    }

    if (sizeof_st_plugin != sizeof(st_mysql_plugin))
    {
      int i;
      for (i= 0;
           ((struct st_maria_plugin *)(ptr + i * sizeof_st_plugin))->info;
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
           (old= (struct st_maria_plugin *)(ptr + i * sizeof_st_plugin))->info;
           i++)
        memcpy(cur + i, old, MY_MIN(sizeof(cur[i]), sizeof_st_plugin));

      sym= cur;
      plugin_dl->allocated= true;
    }
    else
      sym= ptr;
  }
  plugin_dl->plugins= (struct st_maria_plugin *)sym;

  DBUG_RETURN(FALSE);
}