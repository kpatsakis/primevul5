static void plugin_del(struct st_plugin_int *plugin)
{
  DBUG_ENTER("plugin_del");
  mysql_mutex_assert_owner(&LOCK_plugin);
  /* Free allocated strings before deleting the plugin. */
  plugin_vars_free_values(plugin->system_vars);
  restore_ptr_backup(plugin->nbackups, plugin->ptr_backup);
  if (plugin->plugin_dl)
  {
    my_hash_delete(&plugin_hash[plugin->plugin->type], (uchar*)plugin);
    plugin_dl_del(plugin->plugin_dl);
    plugin->state= PLUGIN_IS_FREED;
    free_root(&plugin->mem_root, MYF(0));
  }
  else
    plugin->state= PLUGIN_IS_UNINITIALIZED;
  DBUG_VOID_RETURN;
}