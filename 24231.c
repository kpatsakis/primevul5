static my_bool list_open_tables_callback(TDC_element *element,
                                         list_open_tables_arg *arg)
{
  char *db= (char*) element->m_key;
  char *table_name= (char*) element->m_key + strlen((char*) element->m_key) + 1;

  if (arg->db && my_strcasecmp(system_charset_info, arg->db, db))
    return FALSE;
  if (arg->wild && wild_compare(table_name, arg->wild, 0))
    return FALSE;

  /* Check if user has SELECT privilege for any column in the table */
  arg->table_list.db= db;
  arg->table_list.table_name= table_name;
  arg->table_list.grant.privilege= 0;

  if (check_table_access(arg->thd, SELECT_ACL, &arg->table_list, TRUE, 1, TRUE))
    return FALSE;

  if (!(*arg->start_list= (OPEN_TABLE_LIST *) arg->thd->alloc(
                    sizeof(**arg->start_list) + element->m_key_length)))
    return TRUE;

  strmov((*arg->start_list)->table=
         strmov(((*arg->start_list)->db= (char*) ((*arg->start_list) + 1)),
                db) + 1, table_name);
  (*arg->start_list)->in_use= 0;

  mysql_mutex_lock(&element->LOCK_table_share);
  All_share_tables_list::Iterator it(element->all_tables);
  TABLE *table;
  while ((table= it++))
    if (table->in_use)
      ++(*arg->start_list)->in_use;
  mysql_mutex_unlock(&element->LOCK_table_share);
  (*arg->start_list)->locked= 0;                   /* Obsolete. */
  arg->start_list= &(*arg->start_list)->next;
  *arg->start_list= 0;
  return FALSE;
}