void set_postjoin_aggr_write_func(JOIN_TAB *tab)
{
  JOIN *join= tab->join;
  TABLE *table= tab->table;
  AGGR_OP *aggr= tab->aggr;
  TMP_TABLE_PARAM *tmp_tbl= tab->tmp_table_param;

  DBUG_ASSERT(table && aggr);

  if (table->group && tmp_tbl->sum_func_count && 
      !tmp_tbl->precomputed_group_by)
  {
    /*
      Note for MyISAM tmp tables: if uniques is true keys won't be
      created.
    */
    if (table->s->keys && !table->s->uniques)
    {
      DBUG_PRINT("info",("Using end_update"));
      aggr->set_write_func(end_update);
    }
    else
    {
      DBUG_PRINT("info",("Using end_unique_update"));
      aggr->set_write_func(end_unique_update);
    }
  }
  else if (join->sort_and_group && !tmp_tbl->precomputed_group_by &&
           !join->sort_and_group_aggr_tab && join->tables_list &&
           join->top_join_tab_count)
  {
    DBUG_PRINT("info",("Using end_write_group"));
    aggr->set_write_func(end_write_group);
    join->sort_and_group_aggr_tab= tab;
  }
  else
  {
    DBUG_PRINT("info",("Using end_write"));
    aggr->set_write_func(end_write);
    if (tmp_tbl->precomputed_group_by)
    {
      /*
        A preceding call to create_tmp_table in the case when loose
        index scan is used guarantees that
        TMP_TABLE_PARAM::items_to_copy has enough space for the group
        by functions. It is OK here to use memcpy since we copy
        Item_sum pointers into an array of Item pointers.
      */
      memcpy(tmp_tbl->items_to_copy + tmp_tbl->func_count,
             join->sum_funcs,
             sizeof(Item*)*tmp_tbl->sum_func_count);
      tmp_tbl->items_to_copy[tmp_tbl->func_count+tmp_tbl->sum_func_count]= 0;
    }
  }
}