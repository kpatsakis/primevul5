int plugin_init(int *argc, char **argv, int flags)
{
  uint i;
  struct st_maria_plugin **builtins;
  struct st_maria_plugin *plugin;
  struct st_plugin_int tmp, *plugin_ptr, **reap;
  MEM_ROOT tmp_root;
  bool reaped_mandatory_plugin= false;
  bool mandatory= true;
  LEX_STRING MyISAM= { C_STRING_WITH_LEN("MyISAM") };
  DBUG_ENTER("plugin_init");

  if (initialized)
    DBUG_RETURN(0);

  dlopen_count =0;

  init_alloc_root(&plugin_mem_root, 4096, 4096, MYF(0));
  init_alloc_root(&plugin_vars_mem_root, 4096, 4096, MYF(0));
  init_alloc_root(&tmp_root, 4096, 4096, MYF(0));

  if (my_hash_init(&bookmark_hash, &my_charset_bin, 32, 0, 0,
                   get_bookmark_hash_key, NULL, HASH_UNIQUE))
      goto err;

  /*
    The 80 is from 2016-04-27 when we had 71 default plugins
    Big enough to avoid many mallocs even in future
  */
  if (my_init_dynamic_array(&plugin_dl_array,
                            sizeof(struct st_plugin_dl *), 16, 16, MYF(0)) ||
      my_init_dynamic_array(&plugin_array,
                            sizeof(struct st_plugin_int *), 80, 32, MYF(0)))
    goto err;

  for (i= 0; i < MYSQL_MAX_PLUGIN_TYPE_NUM; i++)
  {
    if (my_hash_init(&plugin_hash[i], system_charset_info, 32, 0, 0,
                     get_plugin_hash_key, NULL, HASH_UNIQUE))
      goto err;
  }

  /* prepare debug_sync service */
  DBUG_ASSERT(strcmp(list_of_services[1].name, "debug_sync_service") == 0);
  list_of_services[1].service= *(void**)&debug_sync_C_callback_ptr;

  /* prepare encryption_keys service */
  finalize_encryption_plugin(0);

  mysql_mutex_lock(&LOCK_plugin);

  initialized= 1;

  /*
    First we register builtin plugins
  */
  if (global_system_variables.log_warnings >= 9)
    sql_print_information("Initializing built-in plugins");

  for (builtins= mysql_mandatory_plugins; *builtins || mandatory; builtins++)
  {
    if (!*builtins)
    {
      builtins= mysql_optional_plugins;
      mandatory= false;
      if (!*builtins)
        break;
    }
    for (plugin= *builtins; plugin->info; plugin++)
    {
      if (opt_ignore_builtin_innodb &&
          !my_strnncoll(&my_charset_latin1, (const uchar*) plugin->name,
                        6, (const uchar*) "InnoDB", 6))
        continue;

      bzero(&tmp, sizeof(tmp));
      tmp.plugin= plugin;
      tmp.name.str= (char *)plugin->name;
      tmp.name.length= strlen(plugin->name);
      tmp.state= 0;
      tmp.load_option= mandatory ? PLUGIN_FORCE : PLUGIN_ON;

      for (i=0; i < array_elements(override_plugin_load_policy); i++)
      {
        if (!my_strcasecmp(&my_charset_latin1, plugin->name,
                           override_plugin_load_policy[i].plugin_name))
        {
          tmp.load_option= override_plugin_load_policy[i].override;
          break;
        }
      }

      free_root(&tmp_root, MYF(MY_MARK_BLOCKS_FREE));
      tmp.state= PLUGIN_IS_UNINITIALIZED;
      if (register_builtin(plugin, &tmp, &plugin_ptr))
        goto err_unlock;
    }
  }

  /*
    First, we initialize only MyISAM - that should almost always succeed
    (almost always, because plugins can be loaded outside of the server, too).
  */
  plugin_ptr= plugin_find_internal(&MyISAM, MYSQL_STORAGE_ENGINE_PLUGIN);
  DBUG_ASSERT(plugin_ptr || !mysql_mandatory_plugins[0]);
  if (plugin_ptr)
  {
    DBUG_ASSERT(plugin_ptr->load_option == PLUGIN_FORCE);

    if (plugin_initialize(&tmp_root, plugin_ptr, argc, argv, false))
      goto err_unlock;

    /*
      set the global default storage engine variable so that it will
      not be null in any child thread.
    */
    global_system_variables.table_plugin =
      intern_plugin_lock(NULL, plugin_int_to_ref(plugin_ptr));
      DBUG_ASSERT(plugin_ptr->ref_count == 1);

  }
  mysql_mutex_unlock(&LOCK_plugin);

  /* Register (not initialize!) all dynamic plugins */
  if (!(flags & PLUGIN_INIT_SKIP_DYNAMIC_LOADING))
  {
    I_List_iterator<i_string> iter(opt_plugin_load_list);
    i_string *item;
    if (global_system_variables.log_warnings >= 9)
      sql_print_information("Initializing plugins specified on the command line");
    while (NULL != (item= iter++))
      plugin_load_list(&tmp_root, item->ptr);

    if (!(flags & PLUGIN_INIT_SKIP_PLUGIN_TABLE))
    {
      char path[FN_REFLEN + 1];
      build_table_filename(path, sizeof(path) - 1, "mysql", "plugin", reg_ext, 0);
      char engine_name_buf[NAME_CHAR_LEN + 1];
      LEX_STRING maybe_myisam= { engine_name_buf, 0 };
      frm_type_enum frm_type= dd_frm_type(NULL, path, &maybe_myisam);
      /* if mysql.plugin table is MyISAM - load it right away */
      if (frm_type == FRMTYPE_TABLE && !strcasecmp(maybe_myisam.str, "MyISAM"))
      {
        plugin_load(&tmp_root);
        flags|= PLUGIN_INIT_SKIP_PLUGIN_TABLE;
      }
    }
  }

  /*
    Now we initialize all remaining plugins
  */

  mysql_mutex_lock(&LOCK_plugin);
  reap= (st_plugin_int **) my_alloca((plugin_array.elements+1) * sizeof(void*));
  *(reap++)= NULL;

  for(;;)
  {
    for (i=0; i < MYSQL_MAX_PLUGIN_TYPE_NUM; i++)
    {
      HASH *hash= plugin_hash + plugin_type_initialization_order[i];
      for (uint idx= 0; idx < hash->records; idx++)
      {
        plugin_ptr= (struct st_plugin_int *) my_hash_element(hash, idx);
        if (plugin_ptr->state == PLUGIN_IS_UNINITIALIZED)
        {
          if (plugin_initialize(&tmp_root, plugin_ptr, argc, argv,
                                (flags & PLUGIN_INIT_SKIP_INITIALIZATION)))
          {
            plugin_ptr->state= PLUGIN_IS_DYING;
            *(reap++)= plugin_ptr;
          }
        }
      }
    }

    /* load and init plugins from the plugin table (unless done already) */
    if (flags & PLUGIN_INIT_SKIP_PLUGIN_TABLE)
      break;

    mysql_mutex_unlock(&LOCK_plugin);
    plugin_load(&tmp_root);
    flags|= PLUGIN_INIT_SKIP_PLUGIN_TABLE;
    mysql_mutex_lock(&LOCK_plugin);
  }

  /*
    Check if any plugins have to be reaped
  */
  while ((plugin_ptr= *(--reap)))
  {
    mysql_mutex_unlock(&LOCK_plugin);
    if (plugin_is_forced(plugin_ptr))
      reaped_mandatory_plugin= TRUE;
    plugin_deinitialize(plugin_ptr, true);
    mysql_mutex_lock(&LOCK_plugin);
    plugin_del(plugin_ptr);
  }

  mysql_mutex_unlock(&LOCK_plugin);
  my_afree(reap);
  if (reaped_mandatory_plugin)
    goto err;

  free_root(&tmp_root, MYF(0));

  DBUG_RETURN(0);

err_unlock:
  mysql_mutex_unlock(&LOCK_plugin);
err:
  free_root(&tmp_root, MYF(0));
  DBUG_RETURN(1);
}