find_variable (name)
     const char *name;
{
  return (find_variable_internal (name, (expanding_redir == 0 && (assigning_in_environment || executing_builtin))));
}