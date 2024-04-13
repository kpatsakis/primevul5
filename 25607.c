int JOIN::save_explain_data_intern(Explain_query *output, 
                                   bool need_tmp_table_arg,
                                   bool need_order_arg, bool distinct_arg, 
                                   const char *message)
{
  JOIN *join= this; /* Legacy: this code used to be a non-member function */
  int cur_error= 0;
  DBUG_ENTER("JOIN::save_explain_data_intern");
  DBUG_PRINT("info", ("Select %p, type %s, message %s",
		      join->select_lex, join->select_lex->type,
		      message ? message : "NULL"));
  DBUG_ASSERT(have_query_plan == QEP_AVAILABLE);
  /* fake_select_lex is created/printed by Explain_union */
  DBUG_ASSERT(join->select_lex != join->unit->fake_select_lex);

  /* There should be no attempts to save query plans for merged selects */
  DBUG_ASSERT(!join->select_lex->master_unit()->derived ||
              join->select_lex->master_unit()->derived->is_materialized_derived() ||
              join->select_lex->master_unit()->derived->is_with_table());

  /* Don't log this into the slow query log */

  if (message)
  {
    explain= new (output->mem_root) Explain_select(output->mem_root, 
                                                   thd->lex->analyze_stmt);
    if (!explain)
      DBUG_RETURN(1); // EoM
#ifndef DBUG_OFF
    explain->select_lex= select_lex;
#endif
    join->select_lex->set_explain_type(true);

    explain->select_id= join->select_lex->select_number;
    explain->select_type= join->select_lex->type;
    explain->using_temporary= need_tmp;
    explain->using_filesort=  need_order_arg;
    /* Setting explain->message means that all other members are invalid */
    explain->message= message;

    if (select_lex->master_unit()->derived)
      explain->connection_type= Explain_node::EXPLAIN_NODE_DERIVED;
    save_agg_explain_data(this, explain);
    output->add_node(explain);
  }
  else if (pushdown_query)
  {
    explain= new (output->mem_root) Explain_select(output->mem_root,
                                                   thd->lex->analyze_stmt);
    select_lex->set_explain_type(true);

    explain->select_id=   select_lex->select_number;
    explain->select_type= select_lex->type;
    explain->using_temporary= need_tmp;
    explain->using_filesort=  need_order_arg;
    explain->message= "Storage engine handles GROUP BY";

    if (select_lex->master_unit()->derived)
      explain->connection_type= Explain_node::EXPLAIN_NODE_DERIVED;
    output->add_node(explain);
  }
  else
  {
    Explain_select *xpl_sel;
    explain= xpl_sel= 
      new (output->mem_root) Explain_select(output->mem_root, 
                                            thd->lex->analyze_stmt);
    table_map used_tables=0;

    join->select_lex->set_explain_type(true);
    xpl_sel->select_id= join->select_lex->select_number;
    xpl_sel->select_type= join->select_lex->type;
    if (select_lex->master_unit()->derived)
      xpl_sel->connection_type= Explain_node::EXPLAIN_NODE_DERIVED;
    
    save_agg_explain_data(this, xpl_sel);

    xpl_sel->exec_const_cond= exec_const_cond;
    xpl_sel->outer_ref_cond= outer_ref_cond;
    xpl_sel->pseudo_bits_cond= pseudo_bits_cond;
    if (tmp_having)
      xpl_sel->having= tmp_having;
    else
      xpl_sel->having= having;
    xpl_sel->having_value= having_value;

    JOIN_TAB* const first_top_tab= join->first_breadth_first_tab();
    JOIN_TAB* prev_bush_root_tab= NULL;

    Explain_basic_join *cur_parent= xpl_sel;
    
    for (JOIN_TAB *tab= first_explain_order_tab(join); tab;
         tab= next_explain_order_tab(join, tab))
    {
      JOIN_TAB *saved_join_tab= NULL;
      TABLE *cur_table= tab->table;

      /* Don't show eliminated tables */
      if (cur_table->map & join->eliminated_tables)
      {
        used_tables|= cur_table->map;
        continue;
      }


      Explain_table_access *eta= (new (output->mem_root)
                                  Explain_table_access(output->mem_root));

      if (tab->bush_root_tab != prev_bush_root_tab)
      {
        if (tab->bush_root_tab)
        {
          /* 
            We've entered an SJ-Materialization nest. Create an object for it.
          */
          cur_parent= new (output->mem_root) Explain_basic_join(output->mem_root);

          JOIN_TAB *first_child= tab->bush_root_tab->bush_children->start;
          cur_parent->select_id=
            first_child->emb_sj_nest->sj_subq_pred->get_identifier();
        }
        else
        {
          /* 
            We've just left an SJ-Materialization nest. We are at the join tab
            that 'embeds the nest'
          */
          DBUG_ASSERT(tab->bush_children);
          eta->sjm_nest= cur_parent;
          cur_parent= xpl_sel;
        }
      }
      prev_bush_root_tab= tab->bush_root_tab;

      cur_parent->add_table(eta, output);
      tab->save_explain_data(eta, used_tables, distinct_arg, first_top_tab);

      if (saved_join_tab)
        tab= saved_join_tab;

      // For next iteration
      used_tables|= cur_table->map;
    }
    output->add_node(xpl_sel);
  }

  for (SELECT_LEX_UNIT *tmp_unit= join->select_lex->first_inner_unit();
       tmp_unit;
       tmp_unit= tmp_unit->next_unit())
  {
    /* 
      Display subqueries only if 
      (1) they are not parts of ON clauses that were eliminated by table 
          elimination.
      (2) they are not merged derived tables
      (3) they are not hanging CTEs (they are needed for execution)
    */
    if (!(tmp_unit->item && tmp_unit->item->eliminated) &&    // (1)
        (!tmp_unit->derived ||
         tmp_unit->derived->is_materialized_derived()) &&     // (2)
        (!tmp_unit->with_element  ||
         (tmp_unit->derived &&
          tmp_unit->derived->derived_result &&
          !tmp_unit->with_element->is_hanging_recursive())))  // (3)
   {
      explain->add_child(tmp_unit->first_select()->select_number);
    }
  }

  if (!cur_error && select_lex->is_top_level_node())
    output->query_plan_ready();

  DBUG_RETURN(cur_error);
}