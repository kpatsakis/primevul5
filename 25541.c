void plugin_unlock_list(THD *thd, plugin_ref *list, uint count)
{
  LEX *lex= thd ? thd->lex : 0;
  DBUG_ENTER("plugin_unlock_list");
  if (count == 0)
    DBUG_VOID_RETURN;

  DBUG_ASSERT(list);
  mysql_mutex_lock(&LOCK_plugin);
  while (count--)
    intern_plugin_unlock(lex, *list++);
  reap_plugins();
  mysql_mutex_unlock(&LOCK_plugin);
  DBUG_VOID_RETURN;
}