new_shell_variable (name)
     const char *name;
{
  SHELL_VAR *entry;

  entry = (SHELL_VAR *)xmalloc (sizeof (SHELL_VAR));

  entry->name = savestring (name);
  var_setvalue (entry, (char *)NULL);
  CLEAR_EXPORTSTR (entry);

  entry->dynamic_value = (sh_var_value_func_t *)NULL;
  entry->assign_func = (sh_var_assign_func_t *)NULL;

  entry->attributes = 0;

  /* Always assume variables are to be made at toplevel!
     make_local_variable has the responsibilty of changing the
     variable context. */
  entry->context = 0;

  return (entry);
}