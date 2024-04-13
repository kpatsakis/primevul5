JOIN::reinit()
{
  DBUG_ENTER("JOIN::reinit");

  unit->offset_limit_cnt= (ha_rows)(select_lex->offset_limit ?
                                    select_lex->offset_limit->val_uint() : 0);

  first_record= false;
  group_sent= false;
  cleaned= false;

  if (aggr_tables)
  {
    JOIN_TAB *curr_tab= join_tab + exec_join_tab_cnt();
    JOIN_TAB *end_tab= curr_tab + aggr_tables;
    for ( ; curr_tab < end_tab; curr_tab++)
    {
      TABLE *tmp_table= curr_tab->table;
      if (!tmp_table->is_created())
        continue;
      tmp_table->file->extra(HA_EXTRA_RESET_STATE);
      tmp_table->file->ha_delete_all_rows();
    }
  }
  clear_sj_tmp_tables(this);
  if (current_ref_ptrs != items0)
  {
    set_items_ref_array(items0);
    set_group_rpa= false;
  }

  /* need to reset ref access state (see join_read_key) */
  if (join_tab)
  {
    JOIN_TAB *tab;
    for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITH_CONST_TABLES); tab;
         tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
    {
      tab->ref.key_err= TRUE;
    }
  }

  /* Reset of sum functions */
  if (sum_funcs)
  {
    Item_sum *func, **func_ptr= sum_funcs;
    while ((func= *(func_ptr++)))
      func->clear();
  }

  if (no_rows_in_result_called)
  {
    /* Reset effect of possible no_rows_in_result() */
    List_iterator_fast<Item> it(fields_list);
    Item *item;
    no_rows_in_result_called= 0;
    while ((item= it++))
      item->restore_to_before_no_rows_in_result();
  }

  if (!(select_options & SELECT_DESCRIBE))
    init_ftfuncs(thd, select_lex, MY_TEST(order));

  DBUG_RETURN(0);
}