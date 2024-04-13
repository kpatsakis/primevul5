uint get_table_def_key(const TABLE_LIST *table_list, const char **key)
{
  /*
    This call relies on the fact that TABLE_LIST::mdl_request::key object
    is properly initialized, so table definition cache can be produced
    from key used by MDL subsystem.
  */
  DBUG_ASSERT(!strcmp(table_list->get_db_name(),
                      table_list->mdl_request.key.db_name()) &&
              !strcmp(table_list->get_table_name(),
                      table_list->mdl_request.key.name()));

  *key= (const char*)table_list->mdl_request.key.ptr() + 1;
  return table_list->mdl_request.key.length() - 1;
}