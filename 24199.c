static my_bool close_cached_connection_tables_callback(
  TDC_element *element, close_cached_connection_tables_arg *arg)
{
  TABLE_LIST *tmp;

  mysql_mutex_lock(&element->LOCK_table_share);
  /* Ignore if table is not open or does not have a connect_string */
  if (!element->share || !element->share->connect_string.length ||
      !element->ref_count)
    goto end;

  /* Compare the connection string */
  if (arg->connection &&
      (arg->connection->length > element->share->connect_string.length ||
       (arg->connection->length < element->share->connect_string.length &&
        (element->share->connect_string.str[arg->connection->length] != '/' &&
         element->share->connect_string.str[arg->connection->length] != '\\')) ||
       strncasecmp(arg->connection->str, element->share->connect_string.str,
                   arg->connection->length)))
    goto end;

  /* close_cached_tables() only uses these elements */
  if (!(tmp= (TABLE_LIST*) alloc_root(arg->thd->mem_root, sizeof(TABLE_LIST))) ||
      !(tmp->db= strdup_root(arg->thd->mem_root, element->share->db.str)) ||
      !(tmp->table_name= strdup_root(arg->thd->mem_root,
                                     element->share->table_name.str)))
  {
    mysql_mutex_unlock(&element->LOCK_table_share);
    return TRUE;
  }

  tmp->next_local= arg->tables;
  arg->tables= tmp;

end:
  mysql_mutex_unlock(&element->LOCK_table_share);
  return FALSE;
}