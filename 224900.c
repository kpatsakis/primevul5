bind_tempenv_variable (name, value)
     const char *name;
     char *value;
{
  SHELL_VAR *var;

  var = temporary_env ? hash_lookup (name, temporary_env) : (SHELL_VAR *)NULL;

  if (var)
    {
      FREE (value_cell (var));
      var_setvalue (var, savestring (value));
      INVALIDATE_EXPORTSTR (var);
    }

  return (var);
}