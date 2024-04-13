void st_select_lex::init_query()
{
  st_select_lex_node::init_query();
  table_list.empty();
  top_join_list.empty();
  join_list= &top_join_list;
  embedding= 0;
  leaf_tables_prep.empty();
  leaf_tables.empty();
  item_list.empty();
  min_max_opt_list.empty();
  join= 0;
  having= prep_having= where= prep_where= 0;
  cond_pushed_into_where= cond_pushed_into_having= 0;
  olap= UNSPECIFIED_OLAP_TYPE;
  having_fix_field= 0;
  having_fix_field_for_pushed_cond= 0;
  context.select_lex= this;
  context.init();
  /*
    Add the name resolution context of the current (sub)query to the
    stack of contexts for the whole query.
    TODO:
    push_context may return an error if there is no memory for a new
    element in the stack, however this method has no return value,
    thus push_context should be moved to a place where query
    initialization is checked for failure.
  */
  parent_lex->push_context(&context, parent_lex->thd->mem_root);
  cond_count= between_count= with_wild= 0;
  max_equal_elems= 0;
  ref_pointer_array.reset();
  select_n_where_fields= 0;
  select_n_reserved= 0;
  select_n_having_items= 0;
  n_sum_items= 0;
  n_child_sum_items= 0;
  hidden_bit_fields= 0;
  fields_in_window_functions= 0;
  subquery_in_having= explicit_limit= 0;
  is_item_list_lookup= 0;
  changed_elements= 0;
  first_natural_join_processing= 1;
  first_cond_optimization= 1;
  parsing_place= NO_MATTER;
  exclude_from_table_unique_test= no_wrap_view_item= FALSE;
  nest_level= 0;
  link_next= 0;
  prep_leaf_list_state= UNINIT;
  have_merged_subqueries= FALSE;
  bzero((char*) expr_cache_may_be_used, sizeof(expr_cache_may_be_used));
  select_list_tables= 0;
  m_non_agg_field_used= false;
  m_agg_func_used= false;
  window_specs.empty();
  window_funcs.empty();
}