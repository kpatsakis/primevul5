void plugin_unlock(THD *thd, plugin_ref plugin)
{
  LEX *lex= thd ? thd->lex : 0;
  DBUG_ENTER("plugin_unlock");
  if (!plugin)
    DBUG_VOID_RETURN;
#ifdef DBUG_OFF
  /* built-in plugins don't need ref counting */
  if (!plugin_dlib(plugin))
    DBUG_VOID_RETURN;
#endif
  mysql_mutex_lock(&LOCK_plugin);
  intern_plugin_unlock(lex, plugin);
  reap_plugins();
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_VOID_RETURN;
}