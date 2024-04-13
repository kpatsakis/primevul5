make_new_variable (name, table)
     const char *name;
     HASH_TABLE *table;
{
  SHELL_VAR *entry;
  BUCKET_CONTENTS *elt;

  entry = new_shell_variable (name);

  /* Make sure we have a shell_variables hash table to add to. */
  if (shell_variables == 0)
    create_variable_tables ();

  elt = hash_insert (savestring (name), table, HASH_NOSRCH);
  elt->data = (PTR_T)entry;

  return entry;
}