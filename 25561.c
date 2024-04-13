static bool plugin_dl_foreach_internal(THD *thd, st_plugin_dl *plugin_dl,
                                       st_maria_plugin *plug,
                                       plugin_foreach_func *func, void *arg)
{
  for (; plug->name; plug++)
  {
    st_plugin_int tmp, *plugin;

    tmp.name.str= const_cast<char*>(plug->name);
    tmp.name.length= strlen(plug->name);
    tmp.plugin= plug;
    tmp.plugin_dl= plugin_dl;

    mysql_mutex_lock(&LOCK_plugin);
    if ((plugin= plugin_find_internal(&tmp.name, MYSQL_ANY_PLUGIN)) &&
        plugin->plugin == plug)

    {
      tmp.state= plugin->state;
      tmp.load_option= plugin->load_option;
    }
    else
    {
      tmp.state= PLUGIN_IS_FREED;
      tmp.load_option= PLUGIN_OFF;
    }
    mysql_mutex_unlock(&LOCK_plugin);

    plugin= &tmp;
    if (func(thd, plugin_int_to_ref(plugin), arg))
      return 1;
  }
  return 0;
}