join_read_const_table(THD *thd, JOIN_TAB *tab, POSITION *pos)
{
  int error;
  TABLE_LIST *tbl;
  DBUG_ENTER("join_read_const_table");
  TABLE *table=tab->table;
  table->const_table=1;
  table->null_row=0;
  table->status=STATUS_NO_RECORD;
  
  if (tab->table->pos_in_table_list->is_materialized_derived() &&
      !tab->table->pos_in_table_list->fill_me)
  {
    //TODO: don't get here at all
    /* Skip materialized derived tables/views. */
    DBUG_RETURN(0);
  }
  else if (tab->table->pos_in_table_list->jtbm_subselect && 
          tab->table->pos_in_table_list->jtbm_subselect->is_jtbm_const_tab)
  {
    /* Row will not be found */
    int res;
    if (tab->table->pos_in_table_list->jtbm_subselect->jtbm_const_row_found)
      res= 0;
    else
      res= -1;
    DBUG_RETURN(res);
  }
  else if (tab->type == JT_SYSTEM)
  {
    if (unlikely((error=join_read_system(tab))))
    {						// Info for DESCRIBE
      tab->info= ET_CONST_ROW_NOT_FOUND;
      /* Mark for EXPLAIN that the row was not found */
      pos->records_read=0.0;
      pos->ref_depend_map= 0;
      if (!table->pos_in_table_list->outer_join || error > 0)
	DBUG_RETURN(error);
    }
    /*
      The optimizer trust the engine that when stats.records is 0, there
      was no found rows
    */
    DBUG_ASSERT(table->file->stats.records > 0 || error);
  }
  else
  {
    if (/*!table->file->key_read && */
        table->covering_keys.is_set(tab->ref.key) && !table->no_keyread &&
        (int) table->reginfo.lock_type <= (int) TL_READ_HIGH_PRIORITY)
    {
      table->file->ha_start_keyread(tab->ref.key);
      tab->index= tab->ref.key;
    }
    error=join_read_const(tab);
    table->file->ha_end_keyread();
    if (unlikely(error))
    {
      tab->info= ET_UNIQUE_ROW_NOT_FOUND;
      /* Mark for EXPLAIN that the row was not found */
      pos->records_read=0.0;
      pos->ref_depend_map= 0;
      if (!table->pos_in_table_list->outer_join || error > 0)
	DBUG_RETURN(error);
    }
  }
  /* 
     Evaluate an on-expression only if it is not considered expensive.
     This mainly prevents executing subqueries in optimization phase.
     This is necessary since proper setup for such execution has not been
     done at this stage.
  */
  if (*tab->on_expr_ref && !table->null_row && 
      !(*tab->on_expr_ref)->is_expensive())
  {
#if !defined(DBUG_OFF) && defined(NOT_USING_ITEM_EQUAL)
    /*
      This test could be very useful to find bugs in the optimizer
      where we would call this function with an expression that can't be
      evaluated yet. We can't have this enabled by default as long as
      have items like Item_equal, that doesn't report they are const but
      they can still be called even if they contain not const items.
    */
    (*tab->on_expr_ref)->update_used_tables();
    DBUG_ASSERT((*tab->on_expr_ref)->const_item());
#endif
    if ((table->null_row= MY_TEST((*tab->on_expr_ref)->val_int() == 0)))
      mark_as_null_row(table);  
  }
  if (!table->null_row && ! tab->join->mixed_implicit_grouping)
    table->maybe_null= 0;

  {
    JOIN *join= tab->join;
    List_iterator<TABLE_LIST> ti(join->select_lex->leaf_tables);
    /* Check appearance of new constant items in Item_equal objects */
    if (join->conds)
      update_const_equal_items(thd, join->conds, tab, TRUE);
    while ((tbl= ti++))
    {
      TABLE_LIST *embedded;
      TABLE_LIST *embedding= tbl;
      do
      {
        embedded= embedding;
        if (embedded->on_expr)
           update_const_equal_items(thd, embedded->on_expr, tab, TRUE);
        embedding= embedded->embedding;
      }
      while (embedding &&
             embedding->nested_join->join_list.head() == embedded);
    }
  }
  DBUG_RETURN(0);
}