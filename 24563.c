THR_LOCK_DATA **ha_maria::store_lock(THD *thd,
                                     THR_LOCK_DATA **to,
                                     enum thr_lock_type lock_type)
{
  /* Test if we can fix test below */
  DBUG_ASSERT(lock_type != TL_UNLOCK &&
              (lock_type == TL_IGNORE || file->lock.type == TL_UNLOCK));
  if (lock_type != TL_IGNORE && file->lock.type == TL_UNLOCK)
  {
    const enum enum_sql_command sql_command= thd->lex->sql_command;
    /*
      We have to disable concurrent inserts for INSERT ... SELECT or
      INSERT/UPDATE/DELETE with sub queries if we are using statement based
      logging.  We take the safe route here and disable this for all commands
      that only does reading that are not SELECT.
    */
    if (lock_type <= TL_READ_HIGH_PRIORITY &&
        !thd->is_current_stmt_binlog_format_row() &&
        (sql_command != SQLCOM_SELECT &&
         sql_command != SQLCOM_LOCK_TABLES) &&
        (thd->variables.option_bits & OPTION_BIN_LOG) &&
        mysql_bin_log.is_open())
      lock_type= TL_READ_NO_INSERT;
    else if (lock_type == TL_WRITE_CONCURRENT_INSERT)
    {
      const enum enum_duplicates duplicates= thd->lex->duplicates;
      /*
        Explanation for the 3 conditions below, in order:

        - Bulk insert may use repair, which will cause problems if other
        threads try to read/insert to the table: disable versioning.
        Note that our read of file->state->records is incorrect, as such
        variable may have changed when we come to start_bulk_insert() (worse
        case: we see != 0 so allow versioning, start_bulk_insert() sees 0 and
        uses repair). This is prevented because start_bulk_insert() will not
        try repair if we enabled versioning.
        - INSERT SELECT ON DUPLICATE KEY UPDATE comes here with
        TL_WRITE_CONCURRENT_INSERT but shouldn't because it can do
        update/delete of a row and versioning doesn't support that
        - same for LOAD DATA CONCURRENT REPLACE.
      */
      if ((file->state->records == 0) ||
          (sql_command == SQLCOM_INSERT_SELECT && duplicates == DUP_UPDATE) ||
          (sql_command == SQLCOM_LOAD && duplicates == DUP_REPLACE))
        lock_type= TL_WRITE;
    }
    file->lock.type= lock_type;
  }
  *to++= &file->lock;
  return to;
}