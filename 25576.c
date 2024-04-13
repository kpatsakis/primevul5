static long *mysql_sys_var_long(THD* thd, int offset)
{
  return (long *) intern_sys_var_ptr(thd, offset, true);
}