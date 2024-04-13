new_var_context (name, flags)
     char *name;
     int flags;
{
  VAR_CONTEXT *vc;

  vc = (VAR_CONTEXT *)xmalloc (sizeof (VAR_CONTEXT));
  vc->name = name ? savestring (name) : (char *)NULL;
  vc->scope = variable_context;
  vc->flags = flags;

  vc->up = vc->down = (VAR_CONTEXT *)NULL;
  vc->table = (HASH_TABLE *)NULL;

  return vc;
}