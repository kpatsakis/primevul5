static char *mysql_sys_var_char(THD* thd, int offset)
{
  return (char *) intern_sys_var_ptr(thd, offset, true);
}