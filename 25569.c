static void update_func_str(THD *thd, struct st_mysql_sys_var *var,
                             void *tgt, const void *save)
{
  char *value= *(char**) save;
  if (var->flags & PLUGIN_VAR_MEMALLOC)
  {
    char *old= *(char**) tgt;
    if (value)
      *(char**) tgt= my_strdup(value, MYF(0));
    else
      *(char**) tgt= 0;
    my_free(old);
  }
  else
    *(char**) tgt= value;
}