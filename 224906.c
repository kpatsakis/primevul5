get_variable_value (var)
     SHELL_VAR *var;
{
  if (var == 0)
    return ((char *)NULL);
#if defined (ARRAY_VARS)
  else if (array_p (var))
    return (array_reference (array_cell (var), 0));
  else if (assoc_p (var))
    return (assoc_reference (assoc_cell (var), "0"));
#endif
  else
    return (value_cell (var));
}