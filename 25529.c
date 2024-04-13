static int construct_options(MEM_ROOT *mem_root, struct st_plugin_int *tmp,
                             my_option *options)
{
  const char *plugin_name= tmp->plugin->name;
  const LEX_STRING plugin_dash = { C_STRING_WITH_LEN("plugin-") };
  uint plugin_name_len= strlen(plugin_name);
  uint optnamelen;
  const int max_comment_len= 255;
  char *comment= (char *) alloc_root(mem_root, max_comment_len + 1);
  char *optname;

  int index= 0, UNINIT_VAR(offset);
  st_mysql_sys_var *opt, **plugin_option;
  st_bookmark *v;

  /** Used to circumvent the const attribute on my_option::name */
  char *plugin_name_ptr, *plugin_name_with_prefix_ptr;

  DBUG_ENTER("construct_options");

  plugin_name_ptr= (char*) alloc_root(mem_root, plugin_name_len + 1);
  strcpy(plugin_name_ptr, plugin_name);
  my_casedn_str(&my_charset_latin1, plugin_name_ptr);
  convert_underscore_to_dash(plugin_name_ptr, plugin_name_len);
  plugin_name_with_prefix_ptr= (char*) alloc_root(mem_root,
                                                  plugin_name_len +
                                                  plugin_dash.length + 1);
  strxmov(plugin_name_with_prefix_ptr, plugin_dash.str, plugin_name_ptr, NullS);

  if (!plugin_is_forced(tmp))
  {
    /* support --skip-plugin-foo syntax */
    options[0].name= plugin_name_ptr;
    options[1].name= plugin_name_with_prefix_ptr;
    options[0].id= options[1].id= 0;
    options[0].var_type= options[1].var_type= GET_ENUM;
    options[0].arg_type= options[1].arg_type= OPT_ARG;
    options[0].def_value= options[1].def_value= 1; /* ON */
    options[0].typelib= options[1].typelib= &global_plugin_typelib;

    strxnmov(comment, max_comment_len, "Enable or disable ", plugin_name,
            " plugin. One of: ON, OFF, FORCE (don't start if the plugin"
            " fails to load), FORCE_PLUS_PERMANENT (like FORCE, but the"
            " plugin can not be uninstalled).", NullS);
    options[0].comment= comment;
    /*
      Allocate temporary space for the value of the tristate.
      This option will have a limited lifetime and is not used beyond
      server initialization.
      GET_ENUM value is an unsigned long integer.
    */
    options[0].value= options[1].value=
                      (uchar **)alloc_root(mem_root, sizeof(ulong));
    *((ulong*) options[0].value)= (ulong) options[0].def_value;

    options+= 2;
  }

  /*
    Two passes as the 2nd pass will take pointer addresses for use
    by my_getopt and register_var() in the first pass uses realloc
  */

  for (plugin_option= tmp->plugin->system_vars;
       plugin_option && *plugin_option; plugin_option++, index++)
  {
    opt= *plugin_option;

    if (!opt->name)
    {
      sql_print_error("Missing variable name in plugin '%s'.",
                      plugin_name);
      DBUG_RETURN(-1);
    }

    if (!(opt->flags & PLUGIN_VAR_THDLOCAL))
      continue;
    if (!(register_var(plugin_name_ptr, opt->name, opt->flags)))
      continue;
    switch (opt->flags & PLUGIN_VAR_TYPEMASK) {
    case PLUGIN_VAR_BOOL:
      ((thdvar_bool_t *) opt)->resolve= mysql_sys_var_char;
      break;
    case PLUGIN_VAR_INT:
      ((thdvar_int_t *) opt)->resolve= mysql_sys_var_int;
      break;
    case PLUGIN_VAR_LONG:
      ((thdvar_long_t *) opt)->resolve= mysql_sys_var_long;
      break;
    case PLUGIN_VAR_LONGLONG:
      ((thdvar_longlong_t *) opt)->resolve= mysql_sys_var_longlong;
      break;
    case PLUGIN_VAR_STR:
      ((thdvar_str_t *) opt)->resolve= mysql_sys_var_str;
      break;
    case PLUGIN_VAR_ENUM:
      ((thdvar_enum_t *) opt)->resolve= mysql_sys_var_ulong;
      break;
    case PLUGIN_VAR_SET:
      ((thdvar_set_t *) opt)->resolve= mysql_sys_var_ulonglong;
      break;
    case PLUGIN_VAR_DOUBLE:
      ((thdvar_double_t *) opt)->resolve= mysql_sys_var_double;
      break;
    default:
      sql_print_error("Unknown variable type code 0x%x in plugin '%s'.",
                      opt->flags, plugin_name);
      DBUG_RETURN(-1);
    };
  }

  for (plugin_option= tmp->plugin->system_vars;
       plugin_option && *plugin_option; plugin_option++, index++)
  {
    switch ((opt= *plugin_option)->flags & PLUGIN_VAR_TYPEMASK) {
    case PLUGIN_VAR_BOOL:
      if (!opt->check)
        opt->check= check_func_bool;
      if (!opt->update)
        opt->update= update_func_bool;
      break;
    case PLUGIN_VAR_INT:
      if (!opt->check)
        opt->check= check_func_int;
      if (!opt->update)
        opt->update= update_func_int;
      break;
    case PLUGIN_VAR_LONG:
      if (!opt->check)
        opt->check= check_func_long;
      if (!opt->update)
        opt->update= update_func_long;
      break;
    case PLUGIN_VAR_LONGLONG:
      if (!opt->check)
        opt->check= check_func_longlong;
      if (!opt->update)
        opt->update= update_func_longlong;
      break;
    case PLUGIN_VAR_STR:
      if (!opt->check)
        opt->check= check_func_str;
      if (!opt->update)
      {
        opt->update= update_func_str;
        if (!(opt->flags & (PLUGIN_VAR_MEMALLOC | PLUGIN_VAR_READONLY)))
        {
          opt->flags|= PLUGIN_VAR_READONLY;
          sql_print_warning("Server variable %s of plugin %s was forced "
                            "to be read-only: string variable without "
                            "update_func and PLUGIN_VAR_MEMALLOC flag",
                            opt->name, plugin_name);
        }
      }
      break;
    case PLUGIN_VAR_ENUM:
      if (!opt->check)
        opt->check= check_func_enum;
      if (!opt->update)
        opt->update= update_func_long;
      break;
    case PLUGIN_VAR_SET:
      if (!opt->check)
        opt->check= check_func_set;
      if (!opt->update)
        opt->update= update_func_longlong;
      break;
    case PLUGIN_VAR_DOUBLE:
      if (!opt->check)
        opt->check= check_func_double;
      if (!opt->update)
        opt->update= update_func_double;
      break;
    default:
      sql_print_error("Unknown variable type code 0x%x in plugin '%s'.",
                      opt->flags, plugin_name);
      DBUG_RETURN(-1);
    }

    if ((opt->flags & (PLUGIN_VAR_NOCMDOPT | PLUGIN_VAR_THDLOCAL))
                    == PLUGIN_VAR_NOCMDOPT)
      continue;

    if (!(opt->flags & PLUGIN_VAR_THDLOCAL))
    {
      optnamelen= strlen(opt->name);
      optname= (char*) alloc_root(mem_root, plugin_name_len + optnamelen + 2);
      strxmov(optname, plugin_name_ptr, "-", opt->name, NullS);
      optnamelen= plugin_name_len + optnamelen + 1;
    }
    else
    {
      /* this should not fail because register_var should create entry */
      if (!(v= find_bookmark(plugin_name_ptr, opt->name, opt->flags)))
      {
        sql_print_error("Thread local variable '%s' not allocated "
                        "in plugin '%s'.", opt->name, plugin_name);
        DBUG_RETURN(-1);
      }

      *(int*)(opt + 1)= offset= v->offset;

      if (opt->flags & PLUGIN_VAR_NOCMDOPT)
      {
        char *val= global_system_variables.dynamic_variables_ptr + offset;
        if (((opt->flags & PLUGIN_VAR_TYPEMASK) == PLUGIN_VAR_STR) &&
             (opt->flags & PLUGIN_VAR_MEMALLOC))
        {
          char *def_val= *(char**)var_def_ptr(opt);
          *(char**)val= def_val ? my_strdup(def_val, MYF(0)) : NULL;
        }
        else
          memcpy(val, var_def_ptr(opt), var_storage_size(opt->flags));
        continue;
      }

      optname= (char*) memdup_root(mem_root, v->key + 1,
                                   (optnamelen= v->name_len) + 1);
    }

    convert_underscore_to_dash(optname, optnamelen);

    options->name= optname;
    options->comment= opt->comment;
    options->app_type= (opt->flags & PLUGIN_VAR_NOSYSVAR) ? NULL : opt;
    options->id= 0;

    plugin_opt_set_limits(options, opt);

    if (opt->flags & PLUGIN_VAR_THDLOCAL)
      options->value= options->u_max_value= (uchar**)
        (global_system_variables.dynamic_variables_ptr + offset);
    else
      options->value= options->u_max_value= *(uchar***) (opt + 1);

    char *option_name_ptr;
    options[1]= options[0];
    options[1].name= option_name_ptr= (char*) alloc_root(mem_root,
                                                        plugin_dash.length +
                                                        optnamelen + 1);
    options[1].comment= 0; /* Hidden from the help text */
    strxmov(option_name_ptr, plugin_dash.str, optname, NullS);

    options+= 2;
  }

  DBUG_RETURN(0);
}