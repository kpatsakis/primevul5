static char **mysql_sys_var_str(THD* thd, int offset)
{
  return (char **) intern_sys_var_ptr(thd, offset, true);
}