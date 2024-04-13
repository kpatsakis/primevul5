void Locked_tables_list::reset()
{
  free_root(&m_locked_tables_root, MYF(0));
  m_locked_tables= NULL;
  m_locked_tables_last= &m_locked_tables;
  m_reopen_array= NULL;
  m_locked_tables_count= 0;
  some_table_marked_for_reopen= 0;
}