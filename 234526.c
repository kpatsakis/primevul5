sub_select(JOIN *join,JOIN_TAB *join_tab,bool end_of_records)
{
  DBUG_ENTER("sub_select");

  if (join_tab->last_inner)
  {
    JOIN_TAB *last_inner_tab= join_tab->last_inner;
    for (JOIN_TAB  *jt= join_tab; jt <= last_inner_tab; jt++)
      jt->table->null_row= 0;
  }
  else
    join_tab->table->null_row=0;

  if (end_of_records)
  {
    enum_nested_loop_state nls=
      (*join_tab->next_select)(join,join_tab+1,end_of_records);
    DBUG_RETURN(nls);
  }
  join_tab->tracker->r_scans++;

  int error;
  enum_nested_loop_state rc= NESTED_LOOP_OK;
  READ_RECORD *info= &join_tab->read_record;


  for (SJ_TMP_TABLE *flush_dups_table= join_tab->flush_weedout_table;
       flush_dups_table;
       flush_dups_table= flush_dups_table->next_flush_table)
  {
    flush_dups_table->sj_weedout_delete_rows();
  }

  if (!join_tab->preread_init_done && join_tab->preread_init())
    DBUG_RETURN(NESTED_LOOP_ERROR);

  join->return_tab= join_tab;

  if (join_tab->last_inner)
  {
    /* join_tab is the first inner table for an outer join operation. */

    /* Set initial state of guard variables for this table.*/
    join_tab->found=0;
    join_tab->not_null_compl= 1;

    /* Set first_unmatched for the last inner table of this group */
    join_tab->last_inner->first_unmatched= join_tab;
    if (join_tab->on_precond && !join_tab->on_precond->val_int())
      rc= NESTED_LOOP_NO_MORE_ROWS;
  }
  join->thd->get_stmt_da()->reset_current_row_for_warning();

  if (rc != NESTED_LOOP_NO_MORE_ROWS && 
      (rc= join_tab_execution_startup(join_tab)) < 0)
    DBUG_RETURN(rc);
  
  if (join_tab->loosescan_match_tab)
    join_tab->loosescan_match_tab->found_match= FALSE;

  if (rc != NESTED_LOOP_NO_MORE_ROWS)
  {
    error= (*join_tab->read_first_record)(join_tab);
    if (!error && join_tab->keep_current_rowid)
      join_tab->table->file->position(join_tab->table->record[0]);    
    rc= evaluate_join_record(join, join_tab, error);
  }

  /* 
    Note: psergey has added the 2nd part of the following condition; the 
    change should probably be made in 5.1, too.
  */
  bool skip_over= FALSE;
  while (rc == NESTED_LOOP_OK && join->return_tab >= join_tab)
  {
    if (join_tab->loosescan_match_tab && 
        join_tab->loosescan_match_tab->found_match)
    {
      KEY *key= join_tab->table->key_info + join_tab->loosescan_key;
      key_copy(join_tab->loosescan_buf, join_tab->table->record[0], key, 
               join_tab->loosescan_key_len);
      skip_over= TRUE;
    }

    error= info->read_record();

    if (skip_over && likely(!error))
    {
      if (!key_cmp(join_tab->table->key_info[join_tab->loosescan_key].key_part,
                   join_tab->loosescan_buf, join_tab->loosescan_key_len))
      {
        /* 
          This is the LooseScan action: skip over records with the same key
          value if we already had a match for them.
        */
        continue;
      }
      join_tab->loosescan_match_tab->found_match= FALSE;
      skip_over= FALSE;
    }

    if (join_tab->keep_current_rowid && likely(!error))
      join_tab->table->file->position(join_tab->table->record[0]);
    
    rc= evaluate_join_record(join, join_tab, error);
  }

  if (rc == NESTED_LOOP_NO_MORE_ROWS &&
      join_tab->last_inner && !join_tab->found)
    rc= evaluate_null_complemented_join_record(join, join_tab);

  if (rc == NESTED_LOOP_NO_MORE_ROWS)
    rc= NESTED_LOOP_OK;
  DBUG_RETURN(rc);
}