static void cleanup_variables(struct system_variables *vars)
{
  st_bookmark *v;
  uint idx;

  mysql_prlock_rdlock(&LOCK_system_variables_hash);
  for (idx= 0; idx < bookmark_hash.records; idx++)
  {
    v= (st_bookmark*) my_hash_element(&bookmark_hash, idx);

    if (v->version > vars->dynamic_variables_version)
      continue; /* not in vars */

    DBUG_ASSERT((uint)v->offset <= vars->dynamic_variables_head);

    /* free allocated strings (PLUGIN_VAR_STR | PLUGIN_VAR_MEMALLOC) */
    if ((v->key[0] & PLUGIN_VAR_TYPEMASK) == PLUGIN_VAR_STR &&
         v->key[0] & BOOKMARK_MEMALLOC)
    {
      char **ptr= (char**)(vars->dynamic_variables_ptr + v->offset);
      my_free(*ptr);
      *ptr= NULL;
    }
  }
  mysql_prlock_unlock(&LOCK_system_variables_hash);

  DBUG_ASSERT(vars->table_plugin == NULL);
  DBUG_ASSERT(vars->tmp_table_plugin == NULL);
  DBUG_ASSERT(vars->enforced_table_plugin == NULL);

  my_free(vars->dynamic_variables_ptr);
  vars->dynamic_variables_ptr= NULL;
  vars->dynamic_variables_size= 0;
  vars->dynamic_variables_version= 0;
}