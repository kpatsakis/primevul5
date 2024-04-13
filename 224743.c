push_func_var (data)
     PTR_T data;
{
  SHELL_VAR *var, *v;

  var = (SHELL_VAR *)data;

  if (tempvar_p (var) && (posixly_correct || (var->attributes & att_propagate)))
    {
      /* Make sure we have a hash table to store the variable in while it is
	 being propagated down to the global variables table.  Create one if
	 we have to */
      if ((vc_isfuncenv (shell_variables) || vc_istempenv (shell_variables)) && shell_variables->table == 0)
	shell_variables->table = hash_create (0);
      /* XXX - should we set v->context here? */
      v = bind_variable_internal (var->name, value_cell (var), shell_variables->table, 0, 0);
      if (shell_variables == global_variables)
	var->attributes &= ~(att_tempvar|att_propagate);
      else
	shell_variables->flags |= VC_HASTMPVAR;
      v->attributes |= var->attributes;
    }
  else
    stupidly_hack_special_variables (var->name);	/* XXX */

  dispose_variable (var);
}