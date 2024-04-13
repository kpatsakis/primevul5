static my_option *construct_help_options(MEM_ROOT *mem_root,
                                         struct st_plugin_int *p)
{
  st_mysql_sys_var **opt;
  my_option *opts;
  uint count= EXTRA_OPTIONS;
  DBUG_ENTER("construct_help_options");

  for (opt= p->plugin->system_vars; opt && *opt; opt++, count+= 2)
    ;

  if (!(opts= (my_option*) alloc_root(mem_root, sizeof(my_option) * count)))
    DBUG_RETURN(NULL);

  bzero(opts, sizeof(my_option) * count);

  /**
    some plugin variables (those that don't have PLUGIN_VAR_NOSYSVAR flag)
    have their names prefixed with the plugin name. Restore the names here
    to get the correct (not double-prefixed) help text.
    We won't need @@sysvars anymore and don't care about their proper names.
  */
  restore_ptr_backup(p->nbackups, p->ptr_backup);

  if (construct_options(mem_root, p, opts))
    DBUG_RETURN(NULL);

  DBUG_RETURN(opts);
}