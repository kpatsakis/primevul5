setup_without_group(THD *thd, Ref_ptr_array ref_pointer_array,
                              TABLE_LIST *tables,
                              List<TABLE_LIST> &leaves,
                              List<Item> &fields,
                              List<Item> &all_fields,
                              COND **conds,
                              ORDER *order,
                              ORDER *group,
                              List<Window_spec> &win_specs,
		              List<Item_window_func> &win_funcs,
                              bool *hidden_group_fields,
                              uint *reserved)
{
  int res;
  enum_parsing_place save_place;
  st_select_lex *const select= thd->lex->current_select;
  nesting_map save_allow_sum_func= thd->lex->allow_sum_func;
  /* 
    Need to stave the value, so we can turn off only any new non_agg_field_used
    additions coming from the WHERE
  */
  const bool saved_non_agg_field_used= select->non_agg_field_used();
  DBUG_ENTER("setup_without_group");

  thd->lex->allow_sum_func.clear_bit(select->nest_level);
  res= setup_conds(thd, tables, leaves, conds);
  if (thd->lex->current_select->first_cond_optimization)
  {
    if (!res && *conds && ! thd->lex->current_select->merged_into)
      (*reserved)= (*conds)->exists2in_reserved_items();
    else
      (*reserved)= 0;
  }

  /* it's not wrong to have non-aggregated columns in a WHERE */
  select->set_non_agg_field_used(saved_non_agg_field_used);

  thd->lex->allow_sum_func.set_bit(select->nest_level);
  
  save_place= thd->lex->current_select->context_analysis_place;
  thd->lex->current_select->context_analysis_place= IN_ORDER_BY;
  res= res || setup_order(thd, ref_pointer_array, tables, fields, all_fields,
                          order);
  thd->lex->allow_sum_func.clear_bit(select->nest_level);
  thd->lex->current_select->context_analysis_place= IN_GROUP_BY;
  res= res || setup_group(thd, ref_pointer_array, tables, fields, all_fields,
                          group, hidden_group_fields);
  thd->lex->current_select->context_analysis_place= save_place;
  thd->lex->allow_sum_func.set_bit(select->nest_level);
  res= res || setup_windows(thd, ref_pointer_array, tables, fields, all_fields,
                            win_specs, win_funcs);
  thd->lex->allow_sum_func= save_allow_sum_func;
  DBUG_RETURN(res);
}