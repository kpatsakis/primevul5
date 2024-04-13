unbind_func (name)
     const char *name;
{
  BUCKET_CONTENTS *elt;
  SHELL_VAR *func;

  elt = hash_remove (name, shell_functions, 0);

  if (elt == 0)
    return -1;

#if defined (PROGRAMMABLE_COMPLETION)
  set_itemlist_dirty (&it_functions);
#endif

  func = (SHELL_VAR *)elt->data;
  if (func)
    {
      if (exported_p (func))
	array_needs_making++;
      dispose_variable (func);
    }

  free (elt->key);
  free (elt);

  return 0;  
}