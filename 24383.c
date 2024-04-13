void lex_start(THD *thd)
{
  LEX *lex= thd->lex;
  DBUG_ENTER("lex_start");
  DBUG_PRINT("info", ("Lex %p", thd->lex));

  lex->thd= lex->unit.thd= thd;

  lex->stmt_lex= lex; // default, should be rewritten for VIEWs And CTEs
  DBUG_ASSERT(!lex->explain);

  lex->context_stack.empty();
  lex->unit.init_query();
  lex->unit.init_select();
  /* 'parent_lex' is used in init_query() so it must be before it. */
  lex->select_lex.parent_lex= lex;
  lex->select_lex.init_query();
  lex->current_select_number= 1;
  lex->curr_with_clause= 0;
  lex->with_clauses_list= 0;
  lex->with_clauses_list_last_next= &lex->with_clauses_list;
  lex->clone_spec_offset= 0;
  lex->value_list.empty();
  lex->update_list.empty();
  lex->set_var_list.empty();
  lex->param_list.empty();
  lex->view_list.empty();
  lex->with_column_list.empty();
  lex->with_persistent_for_clause= FALSE;
  lex->column_list= NULL;
  lex->index_list= NULL;
  lex->prepared_stmt_params.empty();
  lex->auxiliary_table_list.empty();
  lex->unit.next= lex->unit.master=
    lex->unit.link_next= lex->unit.return_to= 0;
  lex->unit.prev= lex->unit.link_prev= 0;
  lex->unit.slave= lex->current_select=
    lex->all_selects_list= &lex->select_lex;
  lex->select_lex.master= &lex->unit;
  lex->select_lex.prev= &lex->unit.slave;
  lex->select_lex.link_next= lex->select_lex.slave= lex->select_lex.next= 0;
  lex->select_lex.link_prev= (st_select_lex_node**)&(lex->all_selects_list);
  lex->select_lex.options= 0;
  lex->select_lex.sql_cache= SELECT_LEX::SQL_CACHE_UNSPECIFIED;
  lex->select_lex.init_order();
  lex->select_lex.group_list.empty();
  if (lex->select_lex.group_list_ptrs)
    lex->select_lex.group_list_ptrs->clear();
  lex->describe= 0;
  lex->analyze_stmt= 0;
  lex->explain_json= false;
  lex->subqueries= FALSE;
  lex->context_analysis_only= 0;
  lex->derived_tables= 0;
  lex->with_cte_resolution= false;
  lex->only_cte_resolution= false;
  lex->safe_to_cache_query= 1;
  lex->parsing_options.reset();
  lex->empty_field_list_on_rset= 0;
  lex->select_lex.select_number= 1;
  lex->part_info= 0;
  lex->select_lex.in_sum_expr=0;
  lex->select_lex.ftfunc_list_alloc.empty();
  lex->select_lex.ftfunc_list= &lex->select_lex.ftfunc_list_alloc;
  lex->select_lex.group_list.empty();
  lex->select_lex.order_list.empty();
  lex->select_lex.gorder_list.empty();
  lex->m_sql_cmd= NULL;
  lex->duplicates= DUP_ERROR;
  lex->ignore= 0;
  lex->spname= NULL;
  lex->spcont= NULL;
  lex->proc_list.first= 0;
  lex->escape_used= FALSE;
  lex->query_tables= 0;
  lex->reset_query_tables_list(FALSE);
  lex->expr_allows_subselect= TRUE;
  lex->use_only_table_context= FALSE;
  lex->parse_vcol_expr= FALSE;
  lex->check_exists= FALSE;
  lex->create_info.lex_start();
  lex->verbose= 0;

  lex->name= null_lex_str;
  lex->event_parse_data= NULL;
  lex->profile_options= PROFILE_NONE;
  lex->nest_level=0 ;
  lex->select_lex.nest_level_base= &lex->unit;
  lex->allow_sum_func= 0;
  lex->in_sum_func= NULL;

  lex->used_tables= 0;
  lex->only_view= FALSE;
  lex->reset_slave_info.all= false;
  lex->limit_rows_examined= 0;
  lex->limit_rows_examined_cnt= ULONGLONG_MAX;
  lex->var_list.empty();
  lex->stmt_var_list.empty();
  lex->proc_list.elements=0;

  lex->save_group_list.empty();
  lex->save_order_list.empty();
  lex->win_ref= NULL;
  lex->win_frame= NULL;
  lex->frame_top_bound= NULL;
  lex->frame_bottom_bound= NULL;
  lex->win_spec= NULL;

  lex->is_lex_started= TRUE;
  DBUG_VOID_RETURN;
}