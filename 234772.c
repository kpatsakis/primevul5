evaluate_null_complemented_join_record(JOIN *join, JOIN_TAB *join_tab)
{
  /*
    The table join_tab is the first inner table of a outer join operation
    and no matches has been found for the current outer row.
  */
  JOIN_TAB *last_inner_tab= join_tab->last_inner;
  /* Cache variables for faster loop */
  COND *select_cond;
  for ( ; join_tab <= last_inner_tab ; join_tab++)
  {
    /* Change the the values of guard predicate variables. */
    join_tab->found= 1;
    join_tab->not_null_compl= 0;
    /* The outer row is complemented by nulls for each inner tables */
    restore_record(join_tab->table,s->default_values);  // Make empty record
    mark_as_null_row(join_tab->table);       // For group by without error
    select_cond= join_tab->select_cond;
    /* Check all attached conditions for inner table rows. */
    if (select_cond && !select_cond->val_int())
      return NESTED_LOOP_OK;
  }
  join_tab--;
  /*
    The row complemented by nulls might be the first row
    of embedding outer joins.
    If so, perform the same actions as in the code
    for the first regular outer join row above.
  */
  for ( ; ; )
  {
    JOIN_TAB *first_unmatched= join_tab->first_unmatched;
    if ((first_unmatched= first_unmatched->first_upper) &&
        first_unmatched->last_inner != join_tab)
      first_unmatched= 0;
    join_tab->first_unmatched= first_unmatched;
    if (!first_unmatched)
      break;
    first_unmatched->found= 1;
    for (JOIN_TAB *tab= first_unmatched; tab <= join_tab; tab++)
    {
      if (tab->select_cond && !tab->select_cond->val_int())
      {
        join->return_tab= tab;
        return NESTED_LOOP_OK;
      }
    }
  }
  /*
    The row complemented by nulls satisfies all conditions
    attached to inner tables.
  */
  if (join_tab->check_weed_out_table)
  {
    int res= join_tab->check_weed_out_table->sj_weedout_check_row(join->thd);
    if (res == -1)
      return NESTED_LOOP_ERROR;
    else if (res == 1)
      return NESTED_LOOP_OK;
  }
  else if (join_tab->do_firstmatch)
  {
    /* 
      We should return to the join_tab->do_firstmatch after we have 
      enumerated all the suffixes for current prefix row combination
    */
    if (join_tab->do_firstmatch < join->return_tab)
      join->return_tab= join_tab->do_firstmatch;
  }

  /*
    Send the row complemented by nulls to be joined with the
    remaining tables.
  */
  return (*join_tab->next_select)(join, join_tab+1, 0);
}