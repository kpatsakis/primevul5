set_if_not (name, value)
     char *name, *value;
{
  SHELL_VAR *v;

  if (shell_variables == 0)
    create_variable_tables ();

  v = find_variable (name);
  if (v == 0)
    v = bind_variable_internal (name, value, global_variables->table, HASH_NOSRCH, 0);
  return (v);
}