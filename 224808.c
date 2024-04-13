pop_var_context ()
{
  VAR_CONTEXT *ret, *vcxt;

  vcxt = shell_variables;
  if (vc_isfuncenv (vcxt) == 0)
    {
      internal_error (_("pop_var_context: head of shell_variables not a function context"));
      return;
    }

  if (ret = vcxt->down)
    {
      ret->up = (VAR_CONTEXT *)NULL;
      shell_variables = ret;
      if (vcxt->table)
	hash_flush (vcxt->table, push_func_var);
      dispose_var_context (vcxt);
    }
  else
    internal_error (_("pop_var_context: no global_variables context"));
}