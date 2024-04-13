free_variable_hash_data (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  dispose_variable (var);
}