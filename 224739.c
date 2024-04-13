dispose_variable_value (var)
     SHELL_VAR *var;
{
  if (function_p (var))
    dispose_command (function_cell (var));
#if defined (ARRAY_VARS)
  else if (array_p (var))
    array_dispose (array_cell (var));
  else if (assoc_p (var))
    assoc_dispose (assoc_cell (var));
#endif
  else
    FREE (value_cell (var));
}