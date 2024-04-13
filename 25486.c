static void update_func_double(THD *thd, struct st_mysql_sys_var *var,
                               void *tgt, const void *save)
{
  *(double *) tgt= *(double *) save;
}