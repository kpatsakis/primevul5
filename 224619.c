pop_scope (is_special)
     int is_special;
{
  VAR_CONTEXT *vcxt, *ret;

  vcxt = shell_variables;
  if (vc_istempscope (vcxt) == 0)
    {
      internal_error (_("pop_scope: head of shell_variables not a temporary environment scope"));
      return;
    }

  ret = vcxt->down;
  if (ret)
    ret->up = (VAR_CONTEXT *)NULL;

  shell_variables = ret;

  /* Now we can take care of merging variables in VCXT into set of scopes
     whose head is RET (shell_variables). */
  FREE (vcxt->name);
  if (vcxt->table)
    {
      if (is_special)
	hash_flush (vcxt->table, push_func_var);
      else
	hash_flush (vcxt->table, push_exported_var);
      hash_dispose (vcxt->table);
    }
  free (vcxt);

  sv_ifs ("IFS");	/* XXX here for now */
}