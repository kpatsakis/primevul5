void Locked_tables_list::add_back_last_deleted_lock(TABLE_LIST *dst_table_list)
{
  /* Link the lock back in the locked tables list */
  dst_table_list->prev_global= m_locked_tables_last;
  *m_locked_tables_last= dst_table_list;
  m_locked_tables_last= &dst_table_list->next_global;
  dst_table_list->next_global= 0;
  m_locked_tables_count++;
}