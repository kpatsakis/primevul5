find_global_variable (name)
     const char *name;
{
  SHELL_VAR *var;

  var = var_lookup (name, global_variables);

  if (var == 0)
    return ((SHELL_VAR *)NULL);

  return (var->dynamic_value ? (*(var->dynamic_value)) (var) : var);
}