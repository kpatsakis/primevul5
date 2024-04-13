bind_variable_value (var, value, aflags)
     SHELL_VAR *var;
     char *value;
     int aflags;
{
  char *t;

  VUNSETATTR (var, att_invisible);

  if (var->assign_func)
    {
      /* If we're appending, we need the old value, so use
	 make_variable_value */
      t = (aflags & ASS_APPEND) ? make_variable_value (var, value, aflags) : value;
      (*(var->assign_func)) (var, t, -1, 0);
      if (t != value && t)
	free (t);      
    }
  else
    {
      t = make_variable_value (var, value, aflags);
      FREE (value_cell (var));
      var_setvalue (var, t);
    }

  INVALIDATE_EXPORTSTR (var);

  if (mark_modified_vars)
    VSETATTR (var, att_exported);

  if (exported_p (var))
    array_needs_making = 1;

  return (var);
}