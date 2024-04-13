bool DML_prelocking_strategy::
handle_view(THD *thd, Query_tables_list *prelocking_ctx,
            TABLE_LIST *table_list, bool *need_prelocking)
{
  if (table_list->view->uses_stored_routines())
  {
    *need_prelocking= TRUE;

    sp_update_stmt_used_routines(thd, prelocking_ctx,
                                 &table_list->view->sroutines_list,
                                 table_list->top_table());
  }

  /*
    If a trigger was defined on one of the associated tables then assign the
    'trg_event_map' value of the view to the next table in table_list. When a
    Stored function is invoked, all the associated tables including the tables
    associated with the trigger are prelocked.
  */
  if (table_list->trg_event_map && table_list->next_global)
    table_list->next_global->trg_event_map= table_list->trg_event_map;
  return FALSE;
}