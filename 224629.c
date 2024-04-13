assign_aliasvar (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  add_alias (key, value);
  return (build_aliasvar (self));
}