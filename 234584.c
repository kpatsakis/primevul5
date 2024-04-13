JOIN::optimize_distinct()
{
  for (JOIN_TAB *last_join_tab= join_tab + top_join_tab_count - 1; ;)
  {
    if (select_lex->select_list_tables & last_join_tab->table->map ||
        last_join_tab->use_join_cache)
      break;
    last_join_tab->shortcut_for_distinct= true;
    if (last_join_tab == join_tab)
      break;
    --last_join_tab;
  }

  /* Optimize "select distinct b from t1 order by key_part_1 limit #" */
  if (order && skip_sort_order)
  {
    /* Should already have been optimized away */
    DBUG_ASSERT(ordered_index_usage == ordered_index_order_by);
    if (ordered_index_usage == ordered_index_order_by)
    {
      order= NULL;
    }
  }
}