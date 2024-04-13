get_lineno (var)
     SHELL_VAR *var;
{
  char *p;
  int ln;

  ln = executing_line_number ();
  p = itos (ln);
  FREE (value_cell (var));
  var_setvalue (var, p);
  return (var);
}