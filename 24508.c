static void update_checkpoint_interval(MYSQL_THD thd,
                                        struct st_mysql_sys_var *var,
                                        void *var_ptr, const void *save)
{
  ma_checkpoint_end();
  ma_checkpoint_init(*(ulong *)var_ptr= (ulong)(*(long *)save));
}