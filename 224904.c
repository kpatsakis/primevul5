kill_all_local_variables ()
{
  VAR_CONTEXT *vc;

  for (vc = shell_variables; vc; vc = vc->down)
    if (vc_isfuncenv (vc) && vc->scope == variable_context)
      break;
  if (vc == 0)
    return;		/* XXX */

  if (vc->table && vc_haslocals (vc))
    {
      delete_all_variables (vc->table);
      hash_dispose (vc->table);
    }
  vc->table = (HASH_TABLE *)NULL;
}