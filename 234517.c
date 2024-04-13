static void choose_initial_table_order(JOIN *join)
{
  TABLE_LIST *emb_subq;
  JOIN_TAB **tab= join->best_ref + join->const_tables;
  JOIN_TAB **tabs_end= tab + join->table_count - join->const_tables;
  DBUG_ENTER("choose_initial_table_order");
  /* Find where the top-level JOIN_TABs end and subquery JOIN_TABs start */
  for (; tab != tabs_end; tab++)
  {
    if ((emb_subq= get_emb_subq(*tab)))
      break;
  }
  uint n_subquery_tabs= (uint)(tabs_end - tab);

  if (!n_subquery_tabs)
    DBUG_VOID_RETURN;

  /* Copy the subquery JOIN_TABs to a separate array */
  JOIN_TAB *subquery_tabs[MAX_TABLES];
  memcpy(subquery_tabs, tab, sizeof(JOIN_TAB*) * n_subquery_tabs);
  
  JOIN_TAB **last_top_level_tab= tab;
  JOIN_TAB **subq_tab= subquery_tabs;
  JOIN_TAB **subq_tabs_end= subquery_tabs + n_subquery_tabs;
  TABLE_LIST *cur_subq_nest= NULL;
  for (; subq_tab < subq_tabs_end; subq_tab++)
  {
    if (get_emb_subq(*subq_tab)!= cur_subq_nest)
    {
      /*
        Reached the part of subquery_tabs that covers tables in some subquery.
      */
      cur_subq_nest= get_emb_subq(*subq_tab);

      /* Determine how many tables the subquery has */
      JOIN_TAB **last_tab_for_subq;
      for (last_tab_for_subq= subq_tab;
           last_tab_for_subq < subq_tabs_end && 
           get_emb_subq(*last_tab_for_subq) == cur_subq_nest;
           last_tab_for_subq++) {}
      uint n_subquery_tables= (uint)(last_tab_for_subq - subq_tab);

      /* 
        Walk the original array and find where this subquery would have been
        attached to
      */
      table_map need_tables= cur_subq_nest->original_subq_pred_used_tables;
      need_tables &= ~(join->const_table_map | PSEUDO_TABLE_BITS);
      for (JOIN_TAB **top_level_tab= join->best_ref + join->const_tables;
           top_level_tab < last_top_level_tab;
           //top_level_tab < join->best_ref + join->table_count;
           top_level_tab++)
      {
        need_tables &= ~(*top_level_tab)->table->map;
        /* Check if this is the place where subquery should be attached */
        if (!need_tables)
        {
          /* Move away the top-level tables that are after top_level_tab */
          size_t top_tail_len= last_top_level_tab - top_level_tab - 1;
          memmove(top_level_tab + 1 + n_subquery_tables, top_level_tab + 1,
                  sizeof(JOIN_TAB*)*top_tail_len);
          last_top_level_tab += n_subquery_tables;
          memcpy(top_level_tab + 1, subq_tab, sizeof(JOIN_TAB*)*n_subquery_tables);
          break;
        }
      }
      DBUG_ASSERT(!need_tables);
      subq_tab += n_subquery_tables - 1;
    }
  }
  DBUG_VOID_RETURN;
}