visible_array_vars (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && array_p (var));
}