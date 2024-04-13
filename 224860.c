visible_var (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0);
}