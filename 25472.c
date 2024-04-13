static int *mysql_sys_var_int(THD* thd, int offset)
{
  return (int *) intern_sys_var_ptr(thd, offset, true);
}