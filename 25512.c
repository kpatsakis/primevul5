static unsigned long *mysql_sys_var_ulong(THD* thd, int offset)
{
  return (unsigned long *) intern_sys_var_ptr(thd, offset, true);
}