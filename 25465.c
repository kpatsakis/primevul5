static int test_plugin_options(MEM_ROOT *tmp_root, struct st_plugin_int *tmp,
                               int *argc, char **argv)
{
  struct sys_var_chain chain= { NULL, NULL };
  bool disable_plugin;
  enum_plugin_load_option plugin_load_option= tmp->load_option;

  MEM_ROOT *mem_root= alloc_root_inited(&tmp->mem_root) ?
                      &tmp->mem_root : &plugin_vars_mem_root;
  st_mysql_sys_var **opt;
  my_option *opts= NULL;
  int error= 1;
  struct st_bookmark *var;
  uint len=0, count= EXTRA_OPTIONS;
  st_ptr_backup *tmp_backup= 0;
  DBUG_ENTER("test_plugin_options");
  DBUG_ASSERT(tmp->plugin && tmp->name.str);

  if (tmp->plugin->system_vars || (*argc > 1))
  {
    for (opt= tmp->plugin->system_vars; opt && *opt; opt++)
    {
      len++;
      if (!((*opt)->flags & PLUGIN_VAR_NOCMDOPT))
        count+= 2; /* --{plugin}-{optname} and --plugin-{plugin}-{optname} */
    }

    if (!(opts= (my_option*) alloc_root(tmp_root, sizeof(my_option) * count)))
    {
      sql_print_error("Out of memory for plugin '%s'.", tmp->name.str);
      DBUG_RETURN(-1);
    }
    bzero(opts, sizeof(my_option) * count);

    if (construct_options(tmp_root, tmp, opts))
    {
      sql_print_error("Bad options for plugin '%s'.", tmp->name.str);
      DBUG_RETURN(-1);
    }

    if (tmp->plugin->system_vars)
    {
      tmp_backup= (st_ptr_backup *)my_alloca(len * sizeof(tmp_backup[0]));
      DBUG_ASSERT(tmp->nbackups == 0);
      DBUG_ASSERT(tmp->ptr_backup == 0);

      for (opt= tmp->plugin->system_vars; *opt; opt++)
      {
        st_mysql_sys_var *o= *opt;
        char *varname;
        sys_var *v;

        if (o->flags & PLUGIN_VAR_NOSYSVAR)
          continue;

        tmp_backup[tmp->nbackups++].save(&o->name);
        if ((var= find_bookmark(tmp->name.str, o->name, o->flags)))
        {
          varname= var->key + 1;
          var->loaded= TRUE;
        }
        else
        {
          var= NULL;
          len= tmp->name.length + strlen(o->name) + 2;
          varname= (char*) alloc_root(mem_root, len);
          strxmov(varname, tmp->name.str, "-", o->name, NullS);
          my_casedn_str(&my_charset_latin1, varname);
          convert_dash_to_underscore(varname, len-1);
        }
        v= new (mem_root) sys_var_pluginvar(&chain, varname, tmp, o);
        v->test_load= (var ? &var->loaded : &static_unload);
        DBUG_ASSERT(static_unload == FALSE);

        if (!(o->flags & PLUGIN_VAR_NOCMDOPT))
        {
          // update app_type, used for I_S.SYSTEM_VARIABLES
          for (my_option *mo=opts; mo->name; mo++)
            if (mo->app_type == o)
              mo->app_type= v;
        }
      }

      if (tmp->nbackups)
      {
        size_t bytes= tmp->nbackups * sizeof(tmp->ptr_backup[0]);
        tmp->ptr_backup= (st_ptr_backup *)alloc_root(mem_root, bytes);
        if (!tmp->ptr_backup)
        {
          restore_ptr_backup(tmp->nbackups, tmp_backup);
          my_afree(tmp_backup);
          goto err;
        }
        memcpy(tmp->ptr_backup, tmp_backup, bytes);
      }
      my_afree(tmp_backup);
    }

    /*
      We adjust the default value to account for the hardcoded exceptions
      we have set for the federated and ndbcluster storage engines.
    */
    if (!plugin_is_forced(tmp))
      opts[0].def_value= opts[1].def_value= plugin_load_option;

    error= handle_options(argc, &argv, opts, mark_changed);
    (*argc)++; /* add back one for the program name */

    if (error)
    {
       sql_print_error("Parsing options for plugin '%s' failed.",
                       tmp->name.str);
       goto err;
    }
    /*
     Set plugin loading policy from option value. First element in the option
     list is always the <plugin name> option value.
    */
    if (!plugin_is_forced(tmp))
      plugin_load_option= (enum_plugin_load_option) *(ulong*) opts[0].value;
  }

  disable_plugin= (plugin_load_option == PLUGIN_OFF);
  tmp->load_option= plugin_load_option;

  error= 1;

  /*
    If the plugin is disabled it should not be initialized.
  */
  if (disable_plugin)
  {
    if (global_system_variables.log_warnings)
      sql_print_information("Plugin '%s' is disabled.",
                            tmp->name.str);
    goto err;
  }

  if (tmp->plugin->system_vars)
  {
    for (opt= tmp->plugin->system_vars; *opt; opt++)
    {
      /*
        PLUGIN_VAR_STR command-line options without PLUGIN_VAR_MEMALLOC, point
        directly to values in the argv[] array. For plugins started at the
        server startup, argv[] array is allocated with load_defaults(), and
        freed when the server is shut down.  But for plugins loaded with
        INSTALL PLUGIN, the memory allocated with load_defaults() is freed with
        free() at the end of mysql_install_plugin(). Which means we cannot
        allow any pointers into that area.
        Thus, for all plugins loaded after the server was started,
        we copy string values to a plugin's memroot.
      */
      if (mysqld_server_started &&
          (((*opt)->flags & (PLUGIN_VAR_TYPEMASK | PLUGIN_VAR_NOCMDOPT |
                             PLUGIN_VAR_MEMALLOC)) == PLUGIN_VAR_STR))
      {
        sysvar_str_t* str= (sysvar_str_t *)*opt;
        if (*str->value)
          *str->value= strdup_root(mem_root, *str->value);
      }
    }

    if (chain.first)
    {
      chain.last->next = NULL;
      if (mysql_add_sys_var_chain(chain.first))
      {
        sql_print_error("Plugin '%s' has conflicting system variables",
                        tmp->name.str);
        goto err;
      }
      tmp->system_vars= chain.first;
    }
  }

  DBUG_RETURN(0);

err:
  if (opts)
    my_cleanup_options(opts);
  DBUG_RETURN(error);
}