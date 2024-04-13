push_temp_var (data)
     PTR_T data;
{
  SHELL_VAR *var, *v;
  HASH_TABLE *binding_table;

  var = (SHELL_VAR *)data;

  binding_table = shell_variables->table;
  if (binding_table == 0)
    {
      if (shell_variables == global_variables)
	/* shouldn't happen */
	binding_table = shell_variables->table = global_variables->table = hash_create (0);
      else
	binding_table = shell_variables->table = hash_create (TEMPENV_HASH_BUCKETS);
    }

  v = bind_variable_internal (var->name, value_cell (var), binding_table, 0, 0);

  /* XXX - should we set the context here?  It shouldn't matter because of how
     assign_in_env works, but might want to check. */
  if (binding_table == global_variables->table)		/* XXX */
    var->attributes &= ~(att_tempvar|att_propagate);
  else
    {
      var->attributes |= att_propagate;
      if  (binding_table == shell_variables->table)
	shell_variables->flags |= VC_HASTMPVAR;
    }
  v->attributes |= var->attributes;

  if (find_special_var (var->name) >= 0)
    tempvar_list[tvlist_ind++] = savestring (var->name);

  dispose_variable (var);
}