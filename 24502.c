int ha_maria::external_lock(THD *thd, int lock_type)
{
  int result= 0, result2;
  DBUG_ENTER("ha_maria::external_lock");
  file->external_ref= (void*) table;            // For ma_killed()
  /*
    We don't test now_transactional because it may vary between lock/unlock
    and thus confuse our reference counting.
    It is critical to skip non-transactional tables: user-visible temporary
    tables get an external_lock() when read/written for the first time, but no
    corresponding unlock (they just stay locked and are later dropped while
    locked); if a tmp table was transactional, "SELECT FROM non_tmp, tmp"
    would never commit as its "locked_tables" count would stay 1.
    When Maria has has_transactions()==TRUE, open_temporary_table()
    (sql_base.cc) will use TRANSACTIONAL_TMP_TABLE and thus the
    external_lock(F_UNLCK) will happen and we can then allow the user to
    create transactional temporary tables.
  */
  if (file->s->base.born_transactional)
  {
    /* Transactional table */
    if (lock_type != F_UNLCK)
    {
      if (file->trn)
      {
        /* This can only happen with tables created with clone() */
        DBUG_PRINT("info",("file->trn: %p", file->trn));
        trnman_increment_locked_tables(file->trn);
      }

      if (!thd->transaction->on)
      {
        /*
          No need to log REDOs/UNDOs. If this is an internal temporary table
          which will be renamed to a permanent table (like in ALTER TABLE),
          the rename happens after unlocking so will be durable (and the table
          will get its create_rename_lsn).
          Note: if we wanted to enable users to have an old backup and apply
          tons of archived logs to roll-forward, we could then not disable
          REDOs/UNDOs in this case.
        */
        DBUG_PRINT("info", ("Disabling logging for table"));
        _ma_tmp_disable_logging_for_table(file, TRUE);
        file->autocommit= 0;
      }
      else
        file->autocommit= !(thd->variables.option_bits &
                            (OPTION_NOT_AUTOCOMMIT | OPTION_BEGIN));
#ifndef ARIA_HAS_TRANSACTIONS
      /*
        Until Aria has full transactions support, including MVCC support for
        delete and update and purging of old states, we have to commit for
        every statement.
      */
      file->autocommit=1;
#endif
    }
    else
    {
      /* We have to test for THD_TRN to protect against implicit commits */
      TRN *trn= (file->trn != &dummy_transaction_object && THD_TRN ? file->trn : 0);
      /* End of transaction */

      /*
        We always re-enable, don't rely on thd->transaction.on as it is
        sometimes reset to true after unlocking (see mysql_truncate() for a
        partitioned table based on Maria).
        Note that we can come here without having an exclusive lock on the
        table, for example in this case:
        external_lock(F_(WR|RD)LCK); thr_lock() which fails due to lock
        abortion; external_lock(F_UNLCK). Fortunately, the re-enabling happens
        only if we were the thread which disabled logging.
      */
      if (_ma_reenable_logging_for_table(file, TRUE))
        DBUG_RETURN(1);
      _ma_reset_trn_for_table(file);
      /*
        Ensure that file->state points to the current number of rows. This
        is needed if someone calls maria_info() without first doing an
        external lock of the table
      */
      file->state= &file->s->state.state;
      if (trn)
      {
        DBUG_PRINT("info",
                   ("locked_tables: %u", trnman_has_locked_tables(trn)));
        DBUG_ASSERT(trnman_has_locked_tables(trn) > 0);
        if (trnman_has_locked_tables(trn) &&
            !trnman_decrement_locked_tables(trn))
        {
          /*
            OK should not have been sent to client yet (ACID).
            This is a bit excessive, ACID requires this only if there are some
            changes to commit (rollback shouldn't be tested).
          */
          DBUG_ASSERT(!thd->get_stmt_da()->is_sent() ||
                      thd->killed);
          /*
            If autocommit, commit transaction. This can happen when open and
            lock tables as part of creating triggers, in which case commit
            is not called.
            Until ARIA_HAS_TRANSACTIONS is not defined, always commit.
          */
          if (file->autocommit)
          {
            if (ma_commit(trn))
              result= HA_ERR_COMMIT_ERROR;
            thd_set_ha_data(thd, maria_hton, 0);
          }
        }
        trnman_set_flags(trn, trnman_get_flags(trn) & ~ TRN_STATE_INFO_LOGGED);
      }
    }
  } /* if transactional table */
  if ((result2= maria_lock_database(file, !table->s->tmp_table ?
                                    lock_type : ((lock_type == F_UNLCK) ?
                                                 F_UNLCK : F_EXTRA_LCK))))
    result= result2;
  if (!file->s->base.born_transactional)
    file->state= &file->s->state.state;         // Restore state if clone

  /* Remember stack end for this thread */
  file->stack_end_ptr= &ha_thd()->mysys_var->stack_ends_here;
  DBUG_RETURN(result);
}