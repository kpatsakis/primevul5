make_new_assoc_variable (name)
     char *name;
{
  SHELL_VAR *entry;
  HASH_TABLE *hash;

  entry = make_new_variable (name, global_variables->table);
  hash = assoc_create (0);

  var_setassoc (entry, hash);
  VSETATTR (entry, att_assoc);
  return entry;
}