static double *mysql_sys_var_double(THD* thd, int offset)
{
  return (double *) intern_sys_var_ptr(thd, offset, true);
}