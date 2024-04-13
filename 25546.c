plugin_ref plugin_lock_by_name(THD *thd, const LEX_STRING *name, int type)
{
  LEX *lex= thd ? thd->lex : 0;
  plugin_ref rc= NULL;
  st_plugin_int *plugin;
  DBUG_ENTER("plugin_lock_by_name");
  mysql_mutex_lock(&LOCK_plugin);
  if ((plugin= plugin_find_internal(name, type)))
    rc= intern_plugin_lock(lex, plugin_int_to_ref(plugin));
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_RETURN(rc);
}