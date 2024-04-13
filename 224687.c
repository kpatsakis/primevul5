visible_and_exported (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && exported_p (var));
}