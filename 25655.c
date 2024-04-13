void save_agg_explain_data(JOIN *join, Explain_select *xpl_sel)
{
  JOIN_TAB *join_tab=join->join_tab + join->exec_join_tab_cnt();
  Explain_aggr_node *prev_node;
  Explain_aggr_node *node= xpl_sel->aggr_tree;
  bool is_analyze= join->thd->lex->analyze_stmt;
  THD *thd= join->thd;

  for (uint i= 0; i < join->aggr_tables; i++, join_tab++)
  {
    // Each aggregate means a temp.table
    prev_node= node;
    node= new (thd->mem_root) Explain_aggr_tmp_table;
    node->child= prev_node;

    if (join_tab->window_funcs_step)
    {
      Explain_aggr_node *new_node= 
        join_tab->window_funcs_step->save_explain_plan(thd->mem_root,
                                                       is_analyze);
      if (new_node)
      {
        prev_node=node;
        node= new_node;
        node->child= prev_node;
      }
    }

    /* The below matches execution in join_init_read_record() */
    if (join_tab->distinct)
    {
      prev_node= node;
      node= new (thd->mem_root) Explain_aggr_remove_dups;
      node->child= prev_node;
    }

    if (join_tab->filesort)
    {
      Explain_aggr_filesort *eaf =
        new (thd->mem_root) Explain_aggr_filesort(thd->mem_root, is_analyze, join_tab->filesort);
      prev_node= node;
      node= eaf;
      node->child= prev_node;
    }
  }
  xpl_sel->aggr_tree= node;
}