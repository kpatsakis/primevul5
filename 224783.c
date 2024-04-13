push_var_context (name, flags, tempvars)
     char *name;
     int flags;
     HASH_TABLE *tempvars;
{
  VAR_CONTEXT *vc;

  vc = new_var_context (name, flags);
  vc->table = tempvars;
  if (tempvars)
    {
      /* Have to do this because the temp environment was created before
	 variable_context was incremented. */
      flatten (tempvars, set_context, (VARLIST *)NULL, 0);
      vc->flags |= VC_HASTMPVAR;
    }
  vc->down = shell_variables;
  shell_variables->up = vc;

  return (shell_variables = vc);
}