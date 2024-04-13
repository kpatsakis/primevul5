void Locked_tables_list::mark_table_for_reopen(THD *thd, TABLE *table)
{
  TABLE_SHARE *share= table->s;

    for (TABLE_LIST *table_list= m_locked_tables;
       table_list; table_list= table_list->next_global)
  {
    if (table_list->table->s == share)
      table_list->table->internal_set_needs_reopen(true);
  }
  /* This is needed in the case where lock tables where not used */
  table->internal_set_needs_reopen(true);
  some_table_marked_for_reopen= 1;
}