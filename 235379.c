static void mark_used_tables_as_free_for_reuse(THD *thd, TABLE *table)
{
  DBUG_ENTER("mark_used_tables_as_free_for_reuse");
  for (; table ; table= table->next)
  {
    DBUG_ASSERT(table->pos_in_locked_tables == NULL ||
                table->pos_in_locked_tables->table == table);
    if (table->query_id == thd->query_id)
    {
      table->query_id= 0;
      table->file->ha_reset();
    }
    else if (table->file->check_table_binlog_row_based_done)
      table->file->clear_cached_table_binlog_row_based_flag();
  }
  DBUG_VOID_RETURN;
}