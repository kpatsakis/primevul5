assign_random (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  sbrand (strtoul (value, (char **)NULL, 10));
  if (subshell_environment)
    seeded_subshell = getpid ();
  return (self);
}