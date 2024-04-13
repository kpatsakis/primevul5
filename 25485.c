static void update_func_int(THD *thd, struct st_mysql_sys_var *var,
                             void *tgt, const void *save)
{
  *(int *)tgt= *(int *) save;
}