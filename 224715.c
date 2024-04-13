assign_dirstack (self, value, ind, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t ind;
     char *key;
{
  set_dirstack_element (ind, 1, value);
  return self;
}