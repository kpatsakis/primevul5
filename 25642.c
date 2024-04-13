JOIN::create_postjoin_aggr_table(JOIN_TAB *tab, List<Item> *table_fields,
                                 ORDER *table_group,
                                 bool save_sum_fields,
                                 bool distinct,
                                 bool keep_row_order)
{
  DBUG_ENTER("JOIN::create_postjoin_aggr_table");
  THD_STAGE_INFO(thd, stage_creating_tmp_table);

  /*
    Pushing LIMIT to the post-join temporary table creation is not applicable
    when there is ORDER BY or GROUP BY or there is no GROUP BY, but
    there are aggregate functions, because in all these cases we need
    all result rows.
  */
  ha_rows table_rows_limit= ((order == NULL || skip_sort_order) &&
                              !table_group &&
                              !select_lex->with_sum_func) ?
                              select_limit : HA_POS_ERROR;

  tab->tmp_table_param= new TMP_TABLE_PARAM(tmp_table_param);
  tab->tmp_table_param->skip_create_table= true;
  TABLE* table= create_tmp_table(thd, tab->tmp_table_param, *table_fields,
                                 table_group, distinct,
                                 save_sum_fields, select_options, table_rows_limit, 
                                 "", true, keep_row_order);
  if (!table)
    DBUG_RETURN(true);
  tmp_table_param.using_outer_summary_function=
    tab->tmp_table_param->using_outer_summary_function;
  tab->join= this;
  DBUG_ASSERT(tab > tab->join->join_tab || !top_join_tab_count ||
              !tables_list);
  tab->table= table;
  if (tab > join_tab)
    (tab - 1)->next_select= sub_select_postjoin_aggr;

  /* if group or order on first table, sort first */
  if ((group_list && simple_group) ||
      (implicit_grouping && select_lex->have_window_funcs()))
  {
    DBUG_PRINT("info",("Sorting for group"));
    THD_STAGE_INFO(thd, stage_sorting_for_group);

    if (ordered_index_usage != ordered_index_group_by &&
        !only_const_tables() &&
        (join_tab + const_tables)->type != JT_CONST && // Don't sort 1 row
        !implicit_grouping &&
        add_sorting_to_table(join_tab + const_tables, group_list))
      goto err;

    if (alloc_group_fields(this, group_list))
      goto err;
    if (make_sum_func_list(all_fields, fields_list, true))
      goto err;
    if (prepare_sum_aggregators(sum_funcs,
                                !(tables_list && 
                                  join_tab->is_using_agg_loose_index_scan())))
      goto err;
    if (setup_sum_funcs(thd, sum_funcs))
      goto err;
    group_list= NULL;
  }
  else
  {
    if (make_sum_func_list(all_fields, fields_list, false))
      goto err;
    if (prepare_sum_aggregators(sum_funcs,
                                !join_tab->is_using_agg_loose_index_scan()))
      goto err;
    if (setup_sum_funcs(thd, sum_funcs))
      goto err;

    if (!group_list && !table->distinct && order && simple_order &&
        tab == join_tab + const_tables)
    {
      DBUG_PRINT("info",("Sorting for order"));
      THD_STAGE_INFO(thd, stage_sorting_for_order);

      if (ordered_index_usage != ordered_index_order_by &&
          !only_const_tables() &&
          add_sorting_to_table(join_tab + const_tables, order))
        goto err;
      order= NULL;
    }
  }
  if (!(tab->aggr= new (thd->mem_root) AGGR_OP(tab)))
    goto err;
  table->reginfo.join_tab= tab;
  DBUG_RETURN(false);

err:
  if (table != NULL)
    free_tmp_table(thd, table);
  tab->table= NULL;
  DBUG_RETURN(true);
}