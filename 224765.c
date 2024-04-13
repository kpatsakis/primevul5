push_scope (flags, tmpvars)
     int flags;
     HASH_TABLE *tmpvars;
{
  return (push_var_context ((char *)NULL, flags, tmpvars));
}