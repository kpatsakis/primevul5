void st_select_lex_unit::init_query()
{
  st_select_lex_node::init_query();
  linkage= GLOBAL_OPTIONS_TYPE;
  select_limit_cnt= HA_POS_ERROR;
  offset_limit_cnt= 0;
  union_distinct= 0;
  prepared= optimized= executed= 0;
  optimize_started= 0;
  item= 0;
  union_result= 0;
  table= 0;
  fake_select_lex= 0;
  saved_fake_select_lex= 0;
  cleaned= 0;
  item_list.empty();
  describe= 0;
  found_rows_for_union= 0;
  derived= 0;
  is_view= false;
  with_clause= 0;
  with_element= 0;
  cloned_from= 0;
  columns_are_renamed= false;
}