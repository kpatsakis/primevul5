void Locked_tables_list::unlink_from_list(THD *thd,
                                          TABLE_LIST *table_list,
                                          bool remove_from_locked_tables)
{
  /*
    If mode is not LTM_LOCK_TABLES, we needn't do anything. Moreover,
    outside this mode pos_in_locked_tables value is not trustworthy.
  */
  if (thd->locked_tables_mode != LTM_LOCK_TABLES &&
      thd->locked_tables_mode != LTM_PRELOCKED_UNDER_LOCK_TABLES)
    return;

  /*
    table_list must be set and point to pos_in_locked_tables of some
    table.
  */
  DBUG_ASSERT(table_list->table->pos_in_locked_tables == table_list);

  /* Clear the pointer, the table will be returned to the table cache. */
  table_list->table->pos_in_locked_tables= NULL;

  /* Mark the table as closed in the locked tables list. */
  table_list->table= NULL;

  /*
    If the table is being dropped or renamed, remove it from
    the locked tables list (implicitly drop the LOCK TABLES lock
    on it).
  */
  if (remove_from_locked_tables)
  {
    *table_list->prev_global= table_list->next_global;
    if (table_list->next_global == NULL)
      m_locked_tables_last= table_list->prev_global;
    else
      table_list->next_global->prev_global= table_list->prev_global;
    m_locked_tables_count--;
  }
}