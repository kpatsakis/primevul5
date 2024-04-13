unbind_function_def (name)
     const char *name;
{
  BUCKET_CONTENTS *elt;
  FUNCTION_DEF *funcdef;

  elt = hash_remove (name, shell_function_defs, 0);

  if (elt == 0)
    return -1;

  funcdef = (FUNCTION_DEF *)elt->data;
  if (funcdef)
    dispose_function_def (funcdef);

  free (elt->key);
  free (elt);

  return 0;  
}