void plugin_shutdown(void)
{
  uint i, count= plugin_array.elements;
  struct st_plugin_int **plugins, *plugin;
  struct st_plugin_dl **dl;
  DBUG_ENTER("plugin_shutdown");

  if (initialized)
  {
    mysql_mutex_lock(&LOCK_plugin);

    reap_needed= true;

    /*
      We want to shut down plugins in a reasonable order, this will
      become important when we have plugins which depend upon each other.
      Circular references cannot be reaped so they are forced afterwards.
      TODO: Have an additional step here to notify all active plugins that
      shutdown is requested to allow plugins to deinitialize in parallel.
    */
    while (reap_needed && (count= plugin_array.elements))
    {
      reap_plugins();
      for (i= 0; i < count; i++)
      {
        plugin= *dynamic_element(&plugin_array, i, struct st_plugin_int **);
        if (plugin->state == PLUGIN_IS_READY)
        {
          plugin->state= PLUGIN_IS_DELETED;
          reap_needed= true;
        }
      }
      if (!reap_needed)
      {
        /*
          release any plugin references held.
        */
        unlock_variables(NULL, &global_system_variables);
        unlock_variables(NULL, &max_system_variables);
      }
    }

    plugins= (struct st_plugin_int **) my_alloca(sizeof(void*) * (count+1));

    /*
      If we have any plugins which did not die cleanly, we force shutdown
    */
    for (i= 0; i < count; i++)
    {
      plugins[i]= *dynamic_element(&plugin_array, i, struct st_plugin_int **);
      /* change the state to ensure no reaping races */
      if (plugins[i]->state == PLUGIN_IS_DELETED)
        plugins[i]->state= PLUGIN_IS_DYING;
    }
    mysql_mutex_unlock(&LOCK_plugin);

    /*
      We loop through all plugins and call deinit() if they have one.
    */
    for (i= 0; i < count; i++)
      if (!(plugins[i]->state & (PLUGIN_IS_UNINITIALIZED | PLUGIN_IS_FREED |
                                 PLUGIN_IS_DISABLED)))
      {
        /*
          We are forcing deinit on plugins so we don't want to do a ref_count
          check until we have processed all the plugins.
        */
        plugin_deinitialize(plugins[i], false);
      }

    /*
      It's perfectly safe not to lock LOCK_plugin, as there're no
      concurrent threads anymore. But some functions called from here
      use mysql_mutex_assert_owner(), so we lock the mutex to satisfy it
    */
    mysql_mutex_lock(&LOCK_plugin);

    /*
      We defer checking ref_counts until after all plugins are deinitialized
      as some may have worker threads holding on to plugin references.
    */
    for (i= 0; i < count; i++)
    {
      if (plugins[i]->ref_count)
        sql_print_error("Plugin '%s' has ref_count=%d after shutdown.",
                        plugins[i]->name.str, plugins[i]->ref_count);
      if (plugins[i]->state & PLUGIN_IS_UNINITIALIZED ||
          plugins[i]->state & PLUGIN_IS_DISABLED)
        plugin_del(plugins[i]);
    }

    /*
      Now we can deallocate all memory.
    */

    cleanup_variables(&global_system_variables);
    cleanup_variables(&max_system_variables);
    mysql_mutex_unlock(&LOCK_plugin);

    initialized= 0;
    mysql_mutex_destroy(&LOCK_plugin);

    my_afree(plugins);
  }

  /* Dispose of the memory */

  for (i= 0; i < MYSQL_MAX_PLUGIN_TYPE_NUM; i++)
    my_hash_free(&plugin_hash[i]);
  delete_dynamic(&plugin_array);

  count= plugin_dl_array.elements;
  dl= (struct st_plugin_dl **)my_alloca(sizeof(void*) * count);
  for (i= 0; i < count; i++)
    dl[i]= *dynamic_element(&plugin_dl_array, i, struct st_plugin_dl **);
  for (i= 0; i < plugin_dl_array.elements; i++)
    free_plugin_mem(dl[i]);
  my_afree(dl);
  delete_dynamic(&plugin_dl_array);

  my_hash_free(&bookmark_hash);
  free_root(&plugin_mem_root, MYF(0));
  free_root(&plugin_vars_mem_root, MYF(0));

  global_variables_dynamic_size= 0;

  DBUG_VOID_RETURN;
}