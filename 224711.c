get_seconds (var)
     SHELL_VAR *var;
{
  time_t time_since_start;
  char *p;

  time_since_start = NOW - shell_start_time;
  p = itos(seconds_value_assigned + time_since_start);

  FREE (value_cell (var));

  VSETATTR (var, att_integer);
  var_setvalue (var, p);
  return (var);
}