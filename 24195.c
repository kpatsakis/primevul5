bool DML_prelocking_strategy::
handle_table(THD *thd, Query_tables_list *prelocking_ctx,
             TABLE_LIST *table_list, bool *need_prelocking)
{
  /* We rely on a caller to check that table is going to be changed. */
  DBUG_ASSERT(table_list->lock_type >= TL_WRITE_ALLOW_WRITE);

  if (table_list->trg_event_map)
  {
    if (table_list->table->triggers)
    {
      *need_prelocking= TRUE;

      if (table_list->table->triggers->
          add_tables_and_routines_for_triggers(thd, prelocking_ctx, table_list))
        return TRUE;
    }
    if (table_list->table->file->referenced_by_foreign_key())
    {
      return (prepare_fk_prelocking_list(thd, prelocking_ctx, table_list,
                                         need_prelocking,
                                         table_list->trg_event_map));
    }
  }
  else if (table_list->slave_fk_event_map &&
           table_list->table->file->referenced_by_foreign_key())
  {
      return (prepare_fk_prelocking_list(thd, prelocking_ctx,
                                         table_list, need_prelocking,
                                         table_list->slave_fk_event_map));

  }
  return FALSE;
}