get_subshell (var)
     SHELL_VAR *var;
{
  char *p;

  p = itos (subshell_level);
  FREE (value_cell (var));
  var_setvalue (var, p);
  return (var);
}