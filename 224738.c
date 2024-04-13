hash_lookup (name, hashed_vars)
     const char *name;
     HASH_TABLE *hashed_vars;
{
  BUCKET_CONTENTS *bucket;

  bucket = hash_search (name, hashed_vars, 0);
  return (bucket ? (SHELL_VAR *)bucket->data : (SHELL_VAR *)NULL);
}