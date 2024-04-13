get_random (var)
     SHELL_VAR *var;
{
  int rv;
  char *p;

  rv = get_random_number ();
  last_random_value = rv;
  p = itos (rv);

  FREE (value_cell (var));

  VSETATTR (var, att_integer);
  var_setvalue (var, p);
  return (var);
}