delete_all_variables (hashed_vars)
     HASH_TABLE *hashed_vars;
{
  hash_flush (hashed_vars, free_variable_hash_data);
}