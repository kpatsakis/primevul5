static SHOW_COMP_OPTION plugin_status(const LEX_STRING *name, int type)
{
  SHOW_COMP_OPTION rc= SHOW_OPTION_NO;
  struct st_plugin_int *plugin;
  DBUG_ENTER("plugin_is_ready");
  mysql_mutex_lock(&LOCK_plugin);
  if ((plugin= plugin_find_internal(name, type)))
  {
    rc= SHOW_OPTION_DISABLED;
    if (plugin->state == PLUGIN_IS_READY)
      rc= SHOW_OPTION_YES;
  }
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_RETURN(rc);
}