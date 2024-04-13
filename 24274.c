Locked_tables_list::init_locked_tables(THD *thd)
{
  DBUG_ASSERT(thd->locked_tables_mode == LTM_NONE);
  DBUG_ASSERT(m_locked_tables == NULL);
  DBUG_ASSERT(m_reopen_array == NULL);
  DBUG_ASSERT(m_locked_tables_count == 0);

  for (TABLE *table= thd->open_tables; table;
       table= table->next, m_locked_tables_count++)
  {
    TABLE_LIST *src_table_list= table->pos_in_table_list;
    char *db, *table_name, *alias;
    size_t db_len=         table->s->db.length;
    size_t table_name_len= table->s->table_name.length;
    size_t alias_len=      table->alias.length();
    TABLE_LIST *dst_table_list;

    if (! multi_alloc_root(&m_locked_tables_root,
                           &dst_table_list, sizeof(*dst_table_list),
                           &db, db_len + 1,
                           &table_name, table_name_len + 1,
                           &alias, alias_len + 1,
                           NullS))
    {
      reset();
      return TRUE;
    }

    memcpy(db,         table->s->db.str, db_len + 1);
    memcpy(table_name, table->s->table_name.str, table_name_len + 1);
    strmake(alias,     table->alias.ptr(), alias_len);
    dst_table_list->init_one_table(db, db_len, table_name, table_name_len,
                                   alias, table->reginfo.lock_type);
    dst_table_list->table= table;
    dst_table_list->mdl_request.ticket= src_table_list->mdl_request.ticket;

    /* Link last into the list of tables */
    *(dst_table_list->prev_global= m_locked_tables_last)= dst_table_list;
    m_locked_tables_last= &dst_table_list->next_global;
    table->pos_in_locked_tables= dst_table_list;
  }
  if (m_locked_tables_count)
  {
    /**
      Allocate an auxiliary array to pass to mysql_lock_tables()
      in reopen_tables(). reopen_tables() is a critical
      path and we don't want to complicate it with extra allocations.
    */
    m_reopen_array= (TABLE_LIST**)alloc_root(&m_locked_tables_root,
                                             sizeof(TABLE_LIST*) *
                                             (m_locked_tables_count+1));
    if (m_reopen_array == NULL)
    {
      reset();
      return TRUE;
    }
  }

  TRANSACT_TRACKER(add_trx_state(thd, TX_LOCKED_TABLES));

  thd->enter_locked_tables_mode(LTM_LOCK_TABLES);

  return FALSE;
}