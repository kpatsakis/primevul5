bool plugin_foreach_with_mask(THD *thd, plugin_foreach_func *func,
                       int type, uint state_mask, void *arg)
{
  uint idx, total= 0;
  struct st_plugin_int *plugin;
  plugin_ref *plugins;
  my_bool res= FALSE;
  DBUG_ENTER("plugin_foreach_with_mask");

  if (!initialized)
    DBUG_RETURN(FALSE);

  mysql_mutex_lock(&LOCK_plugin);
  /*
    Do the alloca out here in case we do have a working alloca:
    leaving the nested stack frame invalidates alloca allocation.
  */
  if (type == MYSQL_ANY_PLUGIN)
  {
    plugins= (plugin_ref*) my_alloca(plugin_array.elements * sizeof(plugin_ref));
    for (idx= 0; idx < plugin_array.elements; idx++)
    {
      plugin= *dynamic_element(&plugin_array, idx, struct st_plugin_int **);
      if ((plugins[total]= intern_plugin_lock(0, plugin_int_to_ref(plugin),
                                              state_mask)))
        total++;
    }
  }
  else
  {
    HASH *hash= plugin_hash + type;
    plugins= (plugin_ref*) my_alloca(hash->records * sizeof(plugin_ref));
    for (idx= 0; idx < hash->records; idx++)
    {
      plugin= (struct st_plugin_int *) my_hash_element(hash, idx);
      if ((plugins[total]= intern_plugin_lock(0, plugin_int_to_ref(plugin),
                                              state_mask)))
        total++;
    }
  }
  mysql_mutex_unlock(&LOCK_plugin);

  for (idx= 0; idx < total; idx++)
  {
    /* It will stop iterating on first engine error when "func" returns TRUE */
    if ((res= func(thd, plugins[idx], arg)))
        break;
  }

  plugin_unlock_list(0, plugins, total);
  my_afree(plugins);
  DBUG_RETURN(res);
}