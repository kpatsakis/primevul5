static bool check_lock_and_start_stmt(THD *thd,
                                      Query_tables_list *prelocking_ctx,
                                      TABLE_LIST *table_list)
{
  int error;
  thr_lock_type lock_type;
  DBUG_ENTER("check_lock_and_start_stmt");

  /*
    Prelocking placeholder is not set for TABLE_LIST that
    are directly used by TOP level statement.
  */
  DBUG_ASSERT(table_list->prelocking_placeholder == false);

  /*
    TL_WRITE_DEFAULT and TL_READ_DEFAULT are supposed to be parser only
    types of locks so they should be converted to appropriate other types
    to be passed to storage engine. The exact lock type passed to the
    engine is important as, for example, InnoDB uses it to determine
    what kind of row locks should be acquired when executing statement
    in prelocked mode or under LOCK TABLES with @@innodb_table_locks = 0.

    Last argument routine_modifies_data for read_lock_type_for_table()
    is ignored, as prelocking placeholder will never be set here.
  */
  DBUG_ASSERT(table_list->prelocking_placeholder == false);
  if (table_list->lock_type == TL_WRITE_DEFAULT)
    lock_type= thd->update_lock_default;
  else if (table_list->lock_type == TL_READ_DEFAULT)
    lock_type= read_lock_type_for_table(thd, prelocking_ctx, table_list, true);
  else
    lock_type= table_list->lock_type;

  if ((int) lock_type > (int) TL_WRITE_ALLOW_WRITE &&
      (int) table_list->table->reginfo.lock_type <= (int) TL_WRITE_ALLOW_WRITE)
  {
    my_error(ER_TABLE_NOT_LOCKED_FOR_WRITE, MYF(0),
             table_list->table->alias.c_ptr());
    DBUG_RETURN(1);
  }
  if ((error= table_list->table->file->start_stmt(thd, lock_type)))
  {
    table_list->table->file->print_error(error, MYF(0));
    DBUG_RETURN(1);
  }

  /*
    Record in transaction state tracking
  */
  TRANSACT_TRACKER(add_trx_state(thd, lock_type,
                                 table_list->table->file->has_transactions()));

  DBUG_RETURN(0);
}