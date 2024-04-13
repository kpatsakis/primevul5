static void reap_plugins(void)
{
  uint count;
  struct st_plugin_int *plugin, **reap, **list;

  mysql_mutex_assert_owner(&LOCK_plugin);

  if (!reap_needed)
    return;

  reap_needed= false;
  count= plugin_array.elements;
  reap= (struct st_plugin_int **)my_alloca(sizeof(plugin)*(count+1));
  *(reap++)= NULL;

  for (uint i=0; i < MYSQL_MAX_PLUGIN_TYPE_NUM; i++)
  {
    HASH *hash= plugin_hash + plugin_type_initialization_order[i];
    for (uint j= 0; j < hash->records; j++)
    {
      plugin= (struct st_plugin_int *) my_hash_element(hash, j);
      if (plugin->state == PLUGIN_IS_DELETED && !plugin->ref_count)
      {
        /* change the status flag to prevent reaping by another thread */
        plugin->state= PLUGIN_IS_DYING;
        *(reap++)= plugin;
      }
    }
  }

  mysql_mutex_unlock(&LOCK_plugin);

  list= reap;
  while ((plugin= *(--list)))
      plugin_deinitialize(plugin, true);

  mysql_mutex_lock(&LOCK_plugin);

  while ((plugin= *(--reap)))
    plugin_del(plugin);

  my_afree(reap);
}