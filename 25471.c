static void update_func_longlong(THD *thd, struct st_mysql_sys_var *var,
                             void *tgt, const void *save)
{
  *(longlong *)tgt= *(ulonglong *) save;
}