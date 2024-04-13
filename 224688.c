var_lookup (name, vcontext)
     const char *name;
     VAR_CONTEXT *vcontext;
{
  VAR_CONTEXT *vc;
  SHELL_VAR *v;

  v = (SHELL_VAR *)NULL;
  for (vc = vcontext; vc; vc = vc->down)
    if (v = hash_lookup (name, vc->table))
      break;

  return v;
}