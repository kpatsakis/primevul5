get_histcmd (var)
     SHELL_VAR *var;
{
  char *p;

  p = itos (history_number ());
  FREE (value_cell (var));
  var_setvalue (var, p);
  return (var);
}