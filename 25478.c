bool plugin_dl_foreach(THD *thd, const LEX_STRING *dl,
                       plugin_foreach_func *func, void *arg)
{
  bool err= 0;

  if (dl)
  {
    mysql_mutex_lock(&LOCK_plugin);
    st_plugin_dl *plugin_dl= plugin_dl_add(dl, REPORT_TO_USER);
    mysql_mutex_unlock(&LOCK_plugin);

    if (!plugin_dl)
      return 1;

    err= plugin_dl_foreach_internal(thd, plugin_dl, plugin_dl->plugins,
                                    func, arg);

    mysql_mutex_lock(&LOCK_plugin);
    plugin_dl_del(plugin_dl);
    mysql_mutex_unlock(&LOCK_plugin);
  }
  else
  {
    struct st_maria_plugin **builtins;
    for (builtins= mysql_mandatory_plugins; !err && *builtins; builtins++)
      err= plugin_dl_foreach_internal(thd, 0, *builtins, func, arg);
    for (builtins= mysql_optional_plugins; !err && *builtins; builtins++)
      err= plugin_dl_foreach_internal(thd, 0, *builtins, func, arg);
  }
  return err;
}