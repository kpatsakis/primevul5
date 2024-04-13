copy_variable (var)
     SHELL_VAR *var;
{
  SHELL_VAR *copy = (SHELL_VAR *)NULL;

  if (var)
    {
      copy = (SHELL_VAR *)xmalloc (sizeof (SHELL_VAR));

      copy->attributes = var->attributes;
      copy->name = savestring (var->name);

      if (function_p (var))
	var_setfunc (copy, copy_command (function_cell (var)));
#if defined (ARRAY_VARS)
      else if (array_p (var))
	var_setarray (copy, array_copy (array_cell (var)));
      else if (assoc_p (var))
	var_setassoc (copy, assoc_copy (assoc_cell (var)));
#endif
      else if (value_cell (var))
	var_setvalue (copy, savestring (value_cell (var)));
      else
	var_setvalue (copy, (char *)NULL);

      copy->dynamic_value = var->dynamic_value;
      copy->assign_func = var->assign_func;

      copy->exportstr = COPY_EXPORTSTR (var);

      copy->context = var->context;
    }
  return (copy);
}