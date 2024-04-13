static void plugin_dl_del(struct st_plugin_dl *plugin_dl)
{
  DBUG_ENTER("plugin_dl_del");

  if (!plugin_dl)
    DBUG_VOID_RETURN;

  mysql_mutex_assert_owner(&LOCK_plugin);

  /* Do not remove this element, unless no other plugin uses this dll. */
  if (! --plugin_dl->ref_count)
  {
    free_plugin_mem(plugin_dl);
    bzero(plugin_dl, sizeof(struct st_plugin_dl));
  }

  DBUG_VOID_RETURN;
}