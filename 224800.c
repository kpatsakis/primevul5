dispose_variable (var)
     SHELL_VAR *var;
{
  if (var == 0)
    return;

  if (nofree_p (var) == 0)
    dispose_variable_value (var);

  FREE_EXPORTSTR (var);

  free (var->name);

  if (exported_p (var))
    array_needs_making = 1;

  free (var);
}