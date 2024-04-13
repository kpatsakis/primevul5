static void update_log_file_size(MYSQL_THD thd,
                                 struct st_mysql_sys_var *var,
                                 void *var_ptr, const void *save)
{
  uint32 size= (uint32)((ulong)(*(long *)save));
  translog_set_file_size(size);
  *(ulong *)var_ptr= size;
}