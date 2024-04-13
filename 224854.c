propagate_temp_var (data)
     PTR_T data;
{
  SHELL_VAR *var;

  var = (SHELL_VAR *)data;
  if (tempvar_p (var) && (var->attributes & att_propagate))
    push_temp_var (data);
  else
    {
      if (find_special_var (var->name) >= 0)
	tempvar_list[tvlist_ind++] = savestring (var->name);
      dispose_variable (var);
    }
}