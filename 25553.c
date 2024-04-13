void sync_dynamic_session_variables(THD* thd, bool global_lock)
{
  uint idx;

  thd->variables.dynamic_variables_ptr= (char*)
    my_realloc(thd->variables.dynamic_variables_ptr,
               global_variables_dynamic_size,
               MYF(MY_WME | MY_FAE | MY_ALLOW_ZERO_PTR));

  if (global_lock)
    mysql_mutex_lock(&LOCK_global_system_variables);

  mysql_mutex_assert_owner(&LOCK_global_system_variables);

  memcpy(thd->variables.dynamic_variables_ptr +
           thd->variables.dynamic_variables_size,
         global_system_variables.dynamic_variables_ptr +
           thd->variables.dynamic_variables_size,
         global_system_variables.dynamic_variables_size -
           thd->variables.dynamic_variables_size);

  /*
    now we need to iterate through any newly copied 'defaults'
    and if it is a string type with MEMALLOC flag, we need to strdup
  */
  for (idx= 0; idx < bookmark_hash.records; idx++)
  {
    st_bookmark *v= (st_bookmark*) my_hash_element(&bookmark_hash,idx);

    if (v->version <= thd->variables.dynamic_variables_version)
      continue; /* already in thd->variables */

    /* Here we do anything special that may be required of the data types */

    if ((v->key[0] & PLUGIN_VAR_TYPEMASK) == PLUGIN_VAR_STR &&
         v->key[0] & BOOKMARK_MEMALLOC)
    {
      char **pp= (char**) (thd->variables.dynamic_variables_ptr + v->offset);
      if (*pp)
        *pp= my_strdup(*pp, MYF(MY_WME|MY_FAE));
    }
  }

  if (global_lock)
    mysql_mutex_unlock(&LOCK_global_system_variables);

  thd->variables.dynamic_variables_version=
         global_system_variables.dynamic_variables_version;
  thd->variables.dynamic_variables_head=
         global_system_variables.dynamic_variables_head;
  thd->variables.dynamic_variables_size=
         global_system_variables.dynamic_variables_size;
}