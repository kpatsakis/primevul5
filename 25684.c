greedy_search(JOIN      *join,
              table_map remaining_tables,
              uint      search_depth,
              uint      prune_level,
              uint      use_cond_selectivity)
{
  double    record_count= 1.0;
  double    read_time=    0.0;
  uint      idx= join->const_tables; // index into 'join->best_ref'
  uint      best_idx;
  uint      size_remain;    // cardinality of remaining_tables
  POSITION  best_pos;
  JOIN_TAB  *best_table; // the next plan node to be added to the curr QEP
  // ==join->tables or # tables in the sj-mat nest we're optimizing
  uint      n_tables __attribute__((unused));
  DBUG_ENTER("greedy_search");

  /* number of tables that remain to be optimized */
  n_tables= size_remain= my_count_bits(remaining_tables &
                                       (join->emb_sjm_nest? 
                                         (join->emb_sjm_nest->sj_inner_tables &
                                          ~join->const_table_map)
                                         :
                                         ~(table_map)0));

  do {
    /* Find the extension of the current QEP with the lowest cost */
    join->best_read= DBL_MAX;
    if (best_extension_by_limited_search(join, remaining_tables, idx, record_count,
                                         read_time, search_depth, prune_level,
                                         use_cond_selectivity))
      DBUG_RETURN(TRUE);
    /*
      'best_read < DBL_MAX' means that optimizer managed to find
      some plan and updated 'best_positions' array accordingly.
    */
    DBUG_ASSERT(join->best_read < DBL_MAX); 

    if (size_remain <= search_depth)
    {
      /*
        'join->best_positions' contains a complete optimal extension of the
        current partial QEP.
      */
      DBUG_EXECUTE("opt", print_plan(join, n_tables,
                                     record_count, read_time, read_time,
                                     "optimal"););
      DBUG_RETURN(FALSE);
    }

    /* select the first table in the optimal extension as most promising */
    best_pos= join->best_positions[idx];
    best_table= best_pos.table;
    /*
      Each subsequent loop of 'best_extension_by_limited_search' uses
      'join->positions' for cost estimates, therefore we have to update its
      value.
    */
    join->positions[idx]= best_pos;

    /*
      Update the interleaving state after extending the current partial plan
      with a new table.
      We are doing this here because best_extension_by_limited_search reverts
      the interleaving state to the one of the non-extended partial plan 
      on exit.
    */
    bool is_interleave_error __attribute__((unused))= 
      check_interleaving_with_nj (best_table);
    /* This has been already checked by best_extension_by_limited_search */
    DBUG_ASSERT(!is_interleave_error);


    /* find the position of 'best_table' in 'join->best_ref' */
    best_idx= idx;
    JOIN_TAB *pos= join->best_ref[best_idx];
    while (pos && best_table != pos)
      pos= join->best_ref[++best_idx];
    DBUG_ASSERT((pos != NULL)); // should always find 'best_table'
    /* move 'best_table' at the first free position in the array of joins */
    swap_variables(JOIN_TAB*, join->best_ref[idx], join->best_ref[best_idx]);

    /* compute the cost of the new plan extended with 'best_table' */
    record_count= COST_MULT(record_count, join->positions[idx].records_read);
    read_time= COST_ADD(read_time,
                         COST_ADD(join->positions[idx].read_time,
                                  record_count / (double) TIME_FOR_COMPARE));

    remaining_tables&= ~(best_table->table->map);
    --size_remain;
    ++idx;

    DBUG_EXECUTE("opt", print_plan(join, idx,
                                   record_count, read_time, read_time,
                                   "extended"););
  } while (TRUE);
}