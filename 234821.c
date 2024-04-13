evaluate_join_record(JOIN *join, JOIN_TAB *join_tab,
                     int error)
{
  bool shortcut_for_distinct= join_tab->shortcut_for_distinct;
  ha_rows found_records=join->found_records;
  COND *select_cond= join_tab->select_cond;
  bool select_cond_result= TRUE;

  DBUG_ENTER("evaluate_join_record");
  DBUG_PRINT("enter",
             ("evaluate_join_record join: %p join_tab: %p"
              " cond: %p error: %d  alias %s",
              join, join_tab, select_cond, error,
              join_tab->table->alias.ptr()));

  if (error > 0 || unlikely(join->thd->is_error())) // Fatal error
    DBUG_RETURN(NESTED_LOOP_ERROR);
  if (error < 0)
    DBUG_RETURN(NESTED_LOOP_NO_MORE_ROWS);
  if (unlikely(join->thd->check_killed()))       // Aborted by user
  {
    DBUG_RETURN(NESTED_LOOP_KILLED);            /* purecov: inspected */
  }

  join_tab->tracker->r_rows++;

  if (select_cond)
  {
    select_cond_result= MY_TEST(select_cond->val_int());

    /* check for errors evaluating the condition */
    if (unlikely(join->thd->is_error()))
      DBUG_RETURN(NESTED_LOOP_ERROR);
  }

  if (!select_cond || select_cond_result)
  {
    /*
      There is no select condition or the attached pushed down
      condition is true => a match is found.
    */
    join_tab->tracker->r_rows_after_where++;

    bool found= 1;
    while (join_tab->first_unmatched && found)
    {
      /*
        The while condition is always false if join_tab is not
        the last inner join table of an outer join operation.
      */
      JOIN_TAB *first_unmatched= join_tab->first_unmatched;
      /*
        Mark that a match for current outer table is found.
        This activates push down conditional predicates attached
        to the all inner tables of the outer join.
      */
      first_unmatched->found= 1;
      for (JOIN_TAB *tab= first_unmatched; tab <= join_tab; tab++)
      {
        /*
          Check whether 'not exists' optimization can be used here.
          If  tab->table->reginfo.not_exists_optimize is set to true
          then WHERE contains a conjunctive predicate IS NULL over
          a non-nullable field of tab. When activated this predicate
          will filter out all records with matches for the left part
          of the outer join whose inner tables start from the
          first_unmatched table and include table tab. To safely use
          'not exists' optimization we have to check that the
          IS NULL predicate is really activated, i.e. all guards
          that wrap it are in the 'open' state. 
	*/  
	bool not_exists_opt_is_applicable=
               tab->table->reginfo.not_exists_optimize;
	for (JOIN_TAB *first_upper= first_unmatched->first_upper;
             not_exists_opt_is_applicable && first_upper;
             first_upper= first_upper->first_upper)
        {
          if (!first_upper->found)
            not_exists_opt_is_applicable= false;
        }
        /* Check all predicates that has just been activated. */
        /*
          Actually all predicates non-guarded by first_unmatched->found
          will be re-evaluated again. It could be fixed, but, probably,
          it's not worth doing now.
        */
        if (tab->select_cond && !tab->select_cond->val_int())
        {
          /* The condition attached to table tab is false */
          if (tab == join_tab)
          {
            found= 0;
            if (not_exists_opt_is_applicable)
              DBUG_RETURN(NESTED_LOOP_NO_MORE_ROWS);
          }            
          else
          {
            /*
              Set a return point if rejected predicate is attached
              not to the last table of the current nest level.
            */
            join->return_tab= tab;
            if (not_exists_opt_is_applicable)
              DBUG_RETURN(NESTED_LOOP_NO_MORE_ROWS);
            else
              DBUG_RETURN(NESTED_LOOP_OK);
          }
        }
      }
      /*
        Check whether join_tab is not the last inner table
        for another embedding outer join.
      */
      if ((first_unmatched= first_unmatched->first_upper) &&
          first_unmatched->last_inner != join_tab)
        first_unmatched= 0;
      join_tab->first_unmatched= first_unmatched;
    }

    JOIN_TAB *return_tab= join->return_tab;
    join_tab->found_match= TRUE;

    if (join_tab->check_weed_out_table && found)
    {
      int res= join_tab->check_weed_out_table->sj_weedout_check_row(join->thd);
      DBUG_PRINT("info", ("weedout_check: %d", res));
      if (res == -1)
        DBUG_RETURN(NESTED_LOOP_ERROR);
      else if (res == 1)
        found= FALSE;
    }
    else if (join_tab->do_firstmatch)
    {
      /* 
        We should return to the join_tab->do_firstmatch after we have 
        enumerated all the suffixes for current prefix row combination
      */
      return_tab= join_tab->do_firstmatch;
    }

    /*
      It was not just a return to lower loop level when one
      of the newly activated predicates is evaluated as false
      (See above join->return_tab= tab).
    */
    join->join_examined_rows++;
    DBUG_PRINT("counts", ("join->examined_rows++: %lu  found: %d",
                          (ulong) join->join_examined_rows, (int) found));

    if (found)
    {
      enum enum_nested_loop_state rc;
      /* A match from join_tab is found for the current partial join. */
      rc= (*join_tab->next_select)(join, join_tab+1, 0);
      join->thd->get_stmt_da()->inc_current_row_for_warning();
      if (rc != NESTED_LOOP_OK && rc != NESTED_LOOP_NO_MORE_ROWS)
        DBUG_RETURN(rc);
      if (return_tab < join->return_tab)
        join->return_tab= return_tab;

      /* check for errors evaluating the condition */
      if (unlikely(join->thd->is_error()))
        DBUG_RETURN(NESTED_LOOP_ERROR);

      if (join->return_tab < join_tab)
        DBUG_RETURN(NESTED_LOOP_OK);
      /*
        Test if this was a SELECT DISTINCT query on a table that
        was not in the field list;  In this case we can abort if
        we found a row, as no new rows can be added to the result.
      */
      if (shortcut_for_distinct && found_records != join->found_records)
        DBUG_RETURN(NESTED_LOOP_NO_MORE_ROWS);
    }
    else
    {
      join->thd->get_stmt_da()->inc_current_row_for_warning();
      join_tab->read_record.unlock_row(join_tab);
    }
  }
  else
  {
    /*
      The condition pushed down to the table join_tab rejects all rows
      with the beginning coinciding with the current partial join.
    */
    join->join_examined_rows++;
    join->thd->get_stmt_da()->inc_current_row_for_warning();
    join_tab->read_record.unlock_row(join_tab);
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}