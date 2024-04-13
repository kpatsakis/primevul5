bind_variable_internal (name, value, table, hflags, aflags)
     const char *name;
     char *value;
     HASH_TABLE *table;
     int hflags, aflags;
{
  char *newval;
  SHELL_VAR *entry;

  entry = (hflags & HASH_NOSRCH) ? (SHELL_VAR *)NULL : hash_lookup (name, table);

  if (entry == 0)
    {
      entry = make_new_variable (name, table);
      var_setvalue (entry, make_variable_value (entry, value, 0)); /* XXX */
    }
  else if (entry->assign_func)	/* array vars have assign functions now */
    {
      INVALIDATE_EXPORTSTR (entry);
      newval = (aflags & ASS_APPEND) ? make_variable_value (entry, value, aflags) : value;
      if (assoc_p (entry))
	entry = (*(entry->assign_func)) (entry, newval, -1, savestring ("0"));
      else if (array_p (entry))
	entry = (*(entry->assign_func)) (entry, newval, 0, 0);
      else
	entry = (*(entry->assign_func)) (entry, newval, -1, 0);
      if (newval != value)
	free (newval);
      return (entry);
    }
  else
    {
      if (readonly_p (entry) || noassign_p (entry))
	{
	  if (readonly_p (entry))
	    err_readonly (name);
	  return (entry);
	}

      /* Variables which are bound are visible. */
      VUNSETATTR (entry, att_invisible);

      newval = make_variable_value (entry, value, aflags);	/* XXX */

      /* Invalidate any cached export string */
      INVALIDATE_EXPORTSTR (entry);

#if defined (ARRAY_VARS)
      /* XXX -- this bears looking at again -- XXX */
      /* If an existing array variable x is being assigned to with x=b or
	 `read x' or something of that nature, silently convert it to
	 x[0]=b or `read x[0]'. */
      if (array_p (entry))
	{
	  array_insert (array_cell (entry), 0, newval);
	  free (newval);
	}
      else if (assoc_p (entry))
	{
	  assoc_insert (assoc_cell (entry), savestring ("0"), newval);
	  free (newval);
	}
      else
#endif
	{
	  FREE (value_cell (entry));
	  var_setvalue (entry, newval);
	}
    }

  if (mark_modified_vars)
    VSETATTR (entry, att_exported);

  if (exported_p (entry))
    array_needs_making = 1;

  return (entry);
}