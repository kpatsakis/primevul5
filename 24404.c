void st_select_lex::init_select()
{
  st_select_lex_node::init_select();
  sj_nests.empty();
  sj_subselects.empty();
  group_list.empty();
  if (group_list_ptrs)
    group_list_ptrs->clear();
  type= db= 0;
  having= 0;
  table_join_options= 0;
  in_sum_expr= with_wild= 0;
  options= 0;
  sql_cache= SQL_CACHE_UNSPECIFIED;
  ftfunc_list_alloc.empty();
  inner_sum_func_list= 0;
  ftfunc_list= &ftfunc_list_alloc;
  linkage= UNSPECIFIED_TYPE;
  order_list.elements= 0;
  order_list.first= 0;
  order_list.next= &order_list.first;
  /* Set limit and offset to default values */
  select_limit= 0;      /* denotes the default limit = HA_POS_ERROR */
  offset_limit= 0;      /* denotes the default offset = 0 */
  with_sum_func= 0;
  with_all_modifier= 0;
  is_correlated= 0;
  cur_pos_in_select_list= UNDEF_POS;
  cond_value= having_value= Item::COND_UNDEF;
  inner_refs_list.empty();
  insert_tables= 0;
  merged_into= 0;
  m_non_agg_field_used= false;
  m_agg_func_used= false;
  name_visibility_map= 0;
  with_dep= 0;
  join= 0;
  lock_type= TL_READ_DEFAULT;
}