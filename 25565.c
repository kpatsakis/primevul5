sys_var *find_sys_var(THD *thd, const char *str, size_t length)
{
  return find_sys_var_ex(thd, str, length, false, false);
}