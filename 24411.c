bool SELECT_LEX::merge_subquery(THD *thd, TABLE_LIST *derived,
                                SELECT_LEX *subq_select,
                                uint table_no, table_map map)
{
  derived->wrap_into_nested_join(subq_select->top_join_list);

  ftfunc_list->append(subq_select->ftfunc_list);
  if (join ||
      thd->lex->sql_command == SQLCOM_UPDATE_MULTI ||
      thd->lex->sql_command == SQLCOM_DELETE_MULTI)
  {
    List_iterator_fast<Item_in_subselect> li(subq_select->sj_subselects);
    Item_in_subselect *in_subq;
    while ((in_subq= li++))
    {
      sj_subselects.push_back(in_subq, thd->mem_root);
      if (in_subq->emb_on_expr_nest == NO_JOIN_NEST)
         in_subq->emb_on_expr_nest= derived;
    }
  }

  /* Walk through child's tables and adjust table map, tablenr,
   * parent_lex */
  subq_select->remap_tables(derived, map, table_no, this);
  subq_select->merged_into= this;

  replace_leaf_table(derived, subq_select->leaf_tables);

  return FALSE;
}