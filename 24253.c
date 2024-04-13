bool Alter_table_prelocking_strategy::
handle_view(THD *thd, Query_tables_list *prelocking_ctx,
            TABLE_LIST *table_list, bool *need_prelocking)
{
  return FALSE;
}