static st_bookmark *register_var(const char *plugin, const char *name,
                                 int flags)
{
  uint length= strlen(plugin) + strlen(name) + 3, size, offset, new_size;
  st_bookmark *result;
  char *varname, *p;

  DBUG_ASSERT(flags & PLUGIN_VAR_THDLOCAL);

  size= var_storage_size(flags);
  varname= ((char*) my_alloca(length));
  strxmov(varname + 1, plugin, "_", name, NullS);
  for (p= varname + 1; *p; p++)
    if (*p == '-')
      *p= '_';

  if (!(result= find_bookmark(NULL, varname + 1, flags)))
  {
    result= (st_bookmark*) alloc_root(&plugin_vars_mem_root,
                                      sizeof(struct st_bookmark) + length-1);
    varname[0]= plugin_var_bookmark_key(flags);
    memcpy(result->key, varname, length);
    result->name_len= length - 2;
    result->offset= -1;

    DBUG_ASSERT(size && !(size & (size-1))); /* must be power of 2 */

    offset= global_system_variables.dynamic_variables_size;
    offset= (offset + size - 1) & ~(size - 1);
    result->offset= (int) offset;

    new_size= (offset + size + 63) & ~63;

    if (new_size > global_variables_dynamic_size)
    {
      global_system_variables.dynamic_variables_ptr= (char*)
        my_realloc(global_system_variables.dynamic_variables_ptr, new_size,
                   MYF(MY_WME | MY_FAE | MY_ALLOW_ZERO_PTR));
      max_system_variables.dynamic_variables_ptr= (char*)
        my_realloc(max_system_variables.dynamic_variables_ptr, new_size,
                   MYF(MY_WME | MY_FAE | MY_ALLOW_ZERO_PTR));
      /*
        Clear the new variable value space. This is required for string
        variables. If their value is non-NULL, it must point to a valid
        string.
      */
      bzero(global_system_variables.dynamic_variables_ptr +
            global_variables_dynamic_size,
            new_size - global_variables_dynamic_size);
      bzero(max_system_variables.dynamic_variables_ptr +
            global_variables_dynamic_size,
            new_size - global_variables_dynamic_size);
      global_variables_dynamic_size= new_size;
    }

    global_system_variables.dynamic_variables_head= offset;
    max_system_variables.dynamic_variables_head= offset;
    global_system_variables.dynamic_variables_size= offset + size;
    max_system_variables.dynamic_variables_size= offset + size;
    global_system_variables.dynamic_variables_version++;
    max_system_variables.dynamic_variables_version++;

    result->version= global_system_variables.dynamic_variables_version;

    /* this should succeed because we have already checked if a dup exists */
    if (my_hash_insert(&bookmark_hash, (uchar*) result))
    {
      fprintf(stderr, "failed to add placeholder to hash");
      DBUG_ASSERT(0);
    }
  }
  my_afree(varname);
  return result;
}