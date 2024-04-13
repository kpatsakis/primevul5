bool extend_table_list(THD *thd, TABLE_LIST *tables,
                       Prelocking_strategy *prelocking_strategy,
                       bool has_prelocking_list)
{
  bool error= false;
  LEX *lex= thd->lex;

  if (thd->locked_tables_mode <= LTM_LOCK_TABLES &&
      ! has_prelocking_list && tables->updating &&
      tables->lock_type >= TL_WRITE_ALLOW_WRITE)
  {
    bool need_prelocking= FALSE;
    TABLE_LIST **save_query_tables_last= lex->query_tables_last;
    /*
      Extend statement's table list and the prelocking set with
      tables and routines according to the current prelocking
      strategy.

      For example, for DML statements we need to add tables and routines
      used by triggers which are going to be invoked for this element of
      table list and also add tables required for handling of foreign keys.
    */
    error= prelocking_strategy->handle_table(thd, lex, tables,
                                             &need_prelocking);

    if (need_prelocking && ! lex->requires_prelocking())
      lex->mark_as_requiring_prelocking(save_query_tables_last);
  }
  return error;
}