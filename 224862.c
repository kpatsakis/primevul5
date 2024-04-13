make_local_variable (name)
     const char *name;
{
  SHELL_VAR *new_var, *old_var;
  VAR_CONTEXT *vc;
  int was_tmpvar;
  char *tmp_value;

  /* local foo; local foo;  is a no-op. */
  old_var = find_variable (name);
  if (old_var && local_p (old_var) && old_var->context == variable_context)
    {
      VUNSETATTR (old_var, att_invisible);
      return (old_var);
    }

  was_tmpvar = old_var && tempvar_p (old_var);
  if (was_tmpvar)
    tmp_value = value_cell (old_var);

  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;

  if (vc == 0)
    {
      internal_error (_("make_local_variable: no function context at current scope"));
      return ((SHELL_VAR *)NULL);
    }
  else if (vc->table == 0)
    vc->table = hash_create (TEMPENV_HASH_BUCKETS);

  /* Since this is called only from the local/declare/typeset code, we can
     call builtin_error here without worry (of course, it will also work
     for anything that sets this_command_name).  Variables with the `noassign'
     attribute may not be made local.  The test against old_var's context
     level is to disallow local copies of readonly global variables (since I
     believe that this could be a security hole).  Readonly copies of calling
     function local variables are OK. */
  if (old_var && (noassign_p (old_var) ||
		 (readonly_p (old_var) && old_var->context == 0)))
    {
      if (readonly_p (old_var))
	sh_readonly (name);
      return ((SHELL_VAR *)NULL);
    }

  if (old_var == 0)
    new_var = make_new_variable (name, vc->table);
  else
    {
      new_var = make_new_variable (name, vc->table);

      /* If we found this variable in one of the temporary environments,
	 inherit its value.  Watch to see if this causes problems with
	 things like `x=4 local x'. */
      if (was_tmpvar)
	var_setvalue (new_var, savestring (tmp_value));

      new_var->attributes = exported_p (old_var) ? att_exported : 0;
    }

  vc->flags |= VC_HASLOCAL;

  new_var->context = variable_context;
  VSETATTR (new_var, att_local);

  if (ifsname (name))
    setifs (new_var);

  return (new_var);
}