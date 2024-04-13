bool Lock_tables_prelocking_strategy::
handle_table(THD *thd, Query_tables_list *prelocking_ctx,
             TABLE_LIST *table_list, bool *need_prelocking)
{
  TABLE_LIST **last= prelocking_ctx->query_tables_last;

  if (DML_prelocking_strategy::handle_table(thd, prelocking_ctx, table_list,
                                            need_prelocking))
    return TRUE;

  /*
    normally we don't need to open FK-prelocked tables for RESTRICT,
    MDL is enough. But under LOCK TABLES we have to open everything
  */
  for (TABLE_LIST *tl= *last; tl; tl= tl->next_global)
    tl->open_strategy= TABLE_LIST::OPEN_NORMAL;

  /* We rely on a caller to check that table is going to be changed. */
  DBUG_ASSERT(table_list->lock_type >= TL_WRITE_ALLOW_WRITE);

  return FALSE;
}