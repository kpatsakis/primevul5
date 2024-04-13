bind_function (name, value)
     const char *name;
     COMMAND *value;
{
  SHELL_VAR *entry;

  entry = find_function (name);
  if (entry == 0)
    {
      BUCKET_CONTENTS *elt;

      elt = hash_insert (savestring (name), shell_functions, HASH_NOSRCH);
      entry = new_shell_variable (name);
      elt->data = (PTR_T)entry;
    }
  else
    INVALIDATE_EXPORTSTR (entry);

  if (var_isset (entry))
    dispose_command (function_cell (entry));

  if (value)
    var_setfunc (entry, copy_command (value));
  else
    var_setfunc (entry, 0);

  VSETATTR (entry, att_function);

  if (mark_modified_vars)
    VSETATTR (entry, att_exported);

  VUNSETATTR (entry, att_invisible);		/* Just to be sure */

  if (exported_p (entry))
    array_needs_making = 1;

#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_functions);
#endif

  return (entry);
}