static unsigned long long *mysql_sys_var_ulonglong(THD* thd, int offset)
{
  return (unsigned long long *) intern_sys_var_ptr(thd, offset, true);
}