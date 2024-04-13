get_string_value (var_name)
     const char *var_name;
{
  SHELL_VAR *var;

  var = find_variable (var_name);
  return ((var) ? get_variable_value (var) : (char *)NULL);
}