static void update_func_bool(THD *thd, struct st_mysql_sys_var *var,
                             void *tgt, const void *save)
{
  *(my_bool *) tgt= *(my_bool *) save ? 1 : 0;
}