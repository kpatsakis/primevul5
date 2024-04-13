assign_hashcmd (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  phash_insert (key, value, 0, 0);
  return (build_hashcmd (self));
}