make_local_array_variable (name, assoc_ok)
     char *name;
     int assoc_ok;
{
  SHELL_VAR *var;
  ARRAY *array;

  var = make_local_variable (name);
  if (var == 0 || array_p (var) || (assoc_ok && assoc_p (var)))
    return var;

  array = array_create ();

  dispose_variable_value (var);
  var_setarray (var, array);
  VSETATTR (var, att_array);
  return var;
}