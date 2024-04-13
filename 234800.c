choose_plan(JOIN *join, table_map join_tables)
{
  uint search_depth= join->thd->variables.optimizer_search_depth;
  uint prune_level=  join->thd->variables.optimizer_prune_level;
  uint use_cond_selectivity= 
         join->thd->variables.optimizer_use_condition_selectivity;
  bool straight_join= MY_TEST(join->select_options & SELECT_STRAIGHT_JOIN);
  DBUG_ENTER("choose_plan");

  join->cur_embedding_map= 0;
  reset_nj_counters(join, join->join_list);
  qsort2_cmp jtab_sort_func;

  if (join->emb_sjm_nest)
  {
    /* We're optimizing semi-join materialization nest, so put the 
       tables from this semi-join as first
    */
    jtab_sort_func= join_tab_cmp_embedded_first;
  }
  else
  {
    /*
      if (SELECT_STRAIGHT_JOIN option is set)
        reorder tables so dependent tables come after tables they depend 
        on, otherwise keep tables in the order they were specified in the query 
      else
        Apply heuristic: pre-sort all access plans with respect to the number of
        records accessed.
    */
    jtab_sort_func= straight_join ? join_tab_cmp_straight : join_tab_cmp;
  }

  /*
    psergey-todo: if we're not optimizing an SJM nest, 
     - sort that outer tables are first, and each sjm nest follows
     - then, put each [sjm_table1, ... sjm_tableN] sub-array right where 
       WHERE clause pushdown would have put it.
  */
  my_qsort2(join->best_ref + join->const_tables,
            join->table_count - join->const_tables, sizeof(JOIN_TAB*),
            jtab_sort_func, (void*)join->emb_sjm_nest);

  if (!join->emb_sjm_nest)
  {
    choose_initial_table_order(join);
  }
  join->cur_sj_inner_tables= 0;

  if (straight_join)
  {
    optimize_straight_join(join, join_tables);
  }
  else
  {
    DBUG_ASSERT(search_depth <= MAX_TABLES + 1);
    if (search_depth == 0)
      /* Automatically determine a reasonable value for 'search_depth' */
      search_depth= determine_search_depth(join);
    if (greedy_search(join, join_tables, search_depth, prune_level,
                      use_cond_selectivity))
      DBUG_RETURN(TRUE);
  }

  /* 
    Store the cost of this query into a user variable
    Don't update last_query_cost for statements that are not "flat joins" :
    i.e. they have subqueries, unions or call stored procedures.
    TODO: calculate a correct cost for a query with subqueries and UNIONs.
  */
  if (join->thd->lex->is_single_level_stmt())
    join->thd->status_var.last_query_cost= join->best_read;
  DBUG_RETURN(FALSE);
}