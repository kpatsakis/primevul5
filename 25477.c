static long long *mysql_sys_var_longlong(THD* thd, int offset)
{
  return (long long *) intern_sys_var_ptr(thd, offset, true);
}