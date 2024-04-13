static int compare_embedding_subqueries(JOIN_TAB *jt1, JOIN_TAB *jt2)
{
  /* Determine if the first table is originally from a subquery */
  TABLE_LIST *tbl1= jt1->table->pos_in_table_list;
  uint tbl1_select_no;
  if (tbl1->jtbm_subselect)
  {
    tbl1_select_no= 
      tbl1->jtbm_subselect->unit->first_select()->select_number;
  }
  else if (tbl1->embedding && tbl1->embedding->sj_subq_pred)
  {
    tbl1_select_no= 
      tbl1->embedding->sj_subq_pred->unit->first_select()->select_number;
  }
  else
    tbl1_select_no= 1; /* Top-level */

  /* Same for the second table */
  TABLE_LIST *tbl2= jt2->table->pos_in_table_list;
  uint tbl2_select_no;
  if (tbl2->jtbm_subselect)
  {
    tbl2_select_no= 
      tbl2->jtbm_subselect->unit->first_select()->select_number;
  }
  else if (tbl2->embedding && tbl2->embedding->sj_subq_pred)
  {
    tbl2_select_no= 
      tbl2->embedding->sj_subq_pred->unit->first_select()->select_number;
  }
  else
    tbl2_select_no= 1; /* Top-level */

  /* 
    Put top-level tables in front. Tables from within subqueries must follow,
    grouped by their owner subquery. We don't care about the order that
    subquery groups are in, because choose_initial_table_order() will re-order
    the groups.
  */
  if (tbl1_select_no != tbl2_select_no)
    return tbl1_select_no > tbl2_select_no ? 1 : -1;
  return 0;
}