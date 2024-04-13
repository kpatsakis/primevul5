make_local_assoc_variable (name)
     char *name;
{
  SHELL_VAR *var;
  HASH_TABLE *hash;

  var = make_local_variable (name);
  if (var == 0 || assoc_p (var))
    return var;

  dispose_variable_value (var);
  hash = assoc_create (0);

  var_setassoc (var, hash);
  VSETATTR (var, att_assoc);
  return var;
}