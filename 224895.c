all_local_variables ()
{
  VARLIST *vlist;
  SHELL_VAR **ret;
  VAR_CONTEXT *vc;

  vc = shell_variables;
  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;

  if (vc == 0)
    {
      internal_error (_("all_local_variables: no function context at current scope"));
      return (SHELL_VAR **)NULL;
    }
  if (vc->table == 0 || HASH_ENTRIES (vc->table) == 0 || vc_haslocals (vc) == 0)
    return (SHELL_VAR **)NULL;
    
  vlist = vlist_alloc (HASH_ENTRIES (vc->table));

  flatten (vc->table, variable_in_context, vlist, 0);

  ret = vlist->list;
  free (vlist);
  if (ret)
    sort_variables (ret);
  return ret;
}