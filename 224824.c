make_new_array_variable (name)
     char *name;
{
  SHELL_VAR *entry;
  ARRAY *array;

  entry = make_new_variable (name, global_variables->table);
  array = array_create ();

  var_setarray (entry, array);
  VSETATTR (entry, att_array);
  return entry;
}