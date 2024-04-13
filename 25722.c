bool multi_update::send_eof()
{
  char buff[STRING_BUFFER_USUAL_SIZE];
  ulonglong id;
  killed_state killed_status= NOT_KILLED;
  DBUG_ENTER("multi_update::send_eof");
  THD_STAGE_INFO(thd, stage_updating_reference_tables);

  /* 
     Does updates for the last n - 1 tables, returns 0 if ok;
     error takes into account killed status gained in do_updates()
  */
  int local_error= thd->is_error();
  if (likely(!local_error))
    local_error = (table_count) ? do_updates() : 0;
  /*
    if local_error is not set ON until after do_updates() then
    later carried out killing should not affect binlogging.
  */
  killed_status= (local_error == 0) ? NOT_KILLED : thd->killed;
  THD_STAGE_INFO(thd, stage_end);

  /* We must invalidate the query cache before binlog writing and
  ha_autocommit_... */

  if (updated)
  {
    query_cache_invalidate3(thd, update_tables, 1);
  }
  /*
    Write the SQL statement to the binlog if we updated
    rows and we succeeded or if we updated some non
    transactional tables.
    
    The query has to binlog because there's a modified non-transactional table
    either from the query's list or via a stored routine: bug#13270,23333
  */

  if (thd->transaction.stmt.modified_non_trans_table)
    thd->transaction.all.modified_non_trans_table= TRUE;
  thd->transaction.all.m_unsafe_rollback_flags|=
    (thd->transaction.stmt.m_unsafe_rollback_flags & THD_TRANS::DID_WAIT);

  if (likely(local_error == 0 ||
             thd->transaction.stmt.modified_non_trans_table))
  {
    if (WSREP_EMULATE_BINLOG(thd) || mysql_bin_log.is_open())
    {
      int errcode= 0;
      if (likely(local_error == 0))
        thd->clear_error();
      else
        errcode= query_error_code(thd, killed_status == NOT_KILLED);

      bool force_stmt= false;
      for (TABLE *table= all_tables->table; table; table= table->next)
      {
        if (table->versioned(VERS_TRX_ID))
        {
          force_stmt= true;
          break;
        }
      }
      enum_binlog_format save_binlog_format;
      save_binlog_format= thd->get_current_stmt_binlog_format();
      if (force_stmt)
        thd->set_current_stmt_binlog_format_stmt();

      if (thd->binlog_query(THD::ROW_QUERY_TYPE, thd->query(),
                            thd->query_length(), transactional_tables, FALSE,
                            FALSE, errcode) > 0)
	local_error= 1;				// Rollback update
      thd->set_current_stmt_binlog_format(save_binlog_format);
    }
  }
  DBUG_ASSERT(trans_safe || !updated ||
              thd->transaction.stmt.modified_non_trans_table);

  if (likely(local_error != 0))
    error_handled= TRUE; // to force early leave from ::abort_result_set()

  if (unlikely(local_error > 0)) // if the above log write did not fail ...
  {
    /* Safety: If we haven't got an error before (can happen in do_updates) */
    my_message(ER_UNKNOWN_ERROR, "An error occurred in multi-table update",
	       MYF(0));
    DBUG_RETURN(TRUE);
  }

  if (!thd->lex->analyze_stmt)
  {
    id= thd->arg_of_last_insert_id_function ?
    thd->first_successful_insert_id_in_prev_stmt : 0;
    my_snprintf(buff, sizeof(buff), ER_THD(thd, ER_UPDATE_INFO),
                (ulong) found, (ulong) updated, (ulong) thd->cuted_fields);
    ::my_ok(thd, (thd->client_capabilities & CLIENT_FOUND_ROWS) ? found : updated,
            id, buff);
  }
  DBUG_RETURN(FALSE);
}