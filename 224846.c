find_tempenv_variable (name)
     const char *name;
{
  return (temporary_env ? hash_lookup (name, temporary_env) : (SHELL_VAR *)NULL);
}