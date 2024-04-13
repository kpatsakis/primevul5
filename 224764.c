makunbound (name, vc)
     const char *name;
     VAR_CONTEXT *vc;
{
  BUCKET_CONTENTS *elt, *new_elt;
  SHELL_VAR *old_var;
  VAR_CONTEXT *v;
  char *t;

  for (elt = (BUCKET_CONTENTS *)NULL, v = vc; v; v = v->down)
    if (elt = hash_remove (name, v->table, 0))
      break;

  if (elt == 0)
    return (-1);

  old_var = (SHELL_VAR *)elt->data;

  if (old_var && exported_p (old_var))
    array_needs_making++;

  /* If we're unsetting a local variable and we're still executing inside
     the function, just mark the variable as invisible.  The function
     eventually called by pop_var_context() will clean it up later.  This
     must be done so that if the variable is subsequently assigned a new
     value inside the function, the `local' attribute is still present.
     We also need to add it back into the correct hash table. */
  if (old_var && local_p (old_var) && variable_context == old_var->context)
    {
      if (nofree_p (old_var))
	var_setvalue (old_var, (char *)NULL);
#if defined (ARRAY_VARS)
      else if (array_p (old_var))
	array_dispose (array_cell (old_var));
      else if (assoc_p (old_var))
	assoc_dispose (assoc_cell (old_var));
#endif
      else
	FREE (value_cell (old_var));
      /* Reset the attributes.  Preserve the export attribute if the variable
	 came from a temporary environment.  Make sure it stays local, and
	 make it invisible. */ 
      old_var->attributes = (exported_p (old_var) && tempvar_p (old_var)) ? att_exported : 0;
      VSETATTR (old_var, att_local);
      VSETATTR (old_var, att_invisible);
      var_setvalue (old_var, (char *)NULL);
      INVALIDATE_EXPORTSTR (old_var);

      new_elt = hash_insert (savestring (old_var->name), v->table, 0);
      new_elt->data = (PTR_T)old_var;
      stupidly_hack_special_variables (old_var->name);

      free (elt->key);
      free (elt);
      return (0);
    }

  /* Have to save a copy of name here, because it might refer to
     old_var->name.  If so, stupidly_hack_special_variables will
     reference freed memory. */
  t = savestring (name);

  free (elt->key);
  free (elt);

  dispose_variable (old_var);
  stupidly_hack_special_variables (t);
  free (t);

  return (0);
}