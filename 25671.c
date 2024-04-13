AGGR_OP::end_send()
{
  enum_nested_loop_state rc= NESTED_LOOP_OK;
  TABLE *table= join_tab->table;
  JOIN *join= join_tab->join;

  // All records were stored, send them further
  int tmp, new_errno= 0;

  if ((rc= put_record(true)) < NESTED_LOOP_OK)
    return rc;

  if ((tmp= table->file->extra(HA_EXTRA_NO_CACHE)))
  {
    DBUG_PRINT("error",("extra(HA_EXTRA_NO_CACHE) failed"));
    new_errno= tmp;
  }
  if ((tmp= table->file->ha_index_or_rnd_end()))
  {
    DBUG_PRINT("error",("ha_index_or_rnd_end() failed"));
    new_errno= tmp;
  }
  if (new_errno)
  {
    table->file->print_error(new_errno,MYF(0));
    return NESTED_LOOP_ERROR;
  }

  // Update ref array
  join_tab->join->set_items_ref_array(*join_tab->ref_array);
  bool keep_last_filesort_result = join_tab->filesort ? false : true;
  if (join_tab->window_funcs_step)
  {
    if (join_tab->window_funcs_step->exec(join, keep_last_filesort_result))
      return NESTED_LOOP_ERROR;
  }

  table->reginfo.lock_type= TL_UNLOCK;

  bool in_first_read= true;

  /*
     Reset the counter before copying rows from internal temporary table to
     INSERT table.
  */
  join_tab->join->thd->get_stmt_da()->reset_current_row_for_warning();
  while (rc == NESTED_LOOP_OK)
  {
    int error;
    if (in_first_read)
    {
      in_first_read= false;
      error= join_init_read_record(join_tab);
    }
    else
      error= join_tab->read_record.read_record(&join_tab->read_record);

    if (error > 0 || (join->thd->is_error()))   // Fatal error
      rc= NESTED_LOOP_ERROR;
    else if (error < 0)
      break;
    else if (join->thd->killed)		  // Aborted by user
    {
      join->thd->send_kill_message();
      rc= NESTED_LOOP_KILLED;
    }
    else
    {
      rc= evaluate_join_record(join, join_tab, 0);
    }
  }

  if (keep_last_filesort_result)
  {
    delete join_tab->filesort_result;
    join_tab->filesort_result= NULL;
  }

  // Finish rnd scn after sending records
  if (join_tab->table->file->inited)
    join_tab->table->file->ha_rnd_end();

  return rc;
}