static uint reset_nj_counters(JOIN *join, List<TABLE_LIST> *join_list)
{
  List_iterator<TABLE_LIST> li(*join_list);
  TABLE_LIST *table;
  DBUG_ENTER("reset_nj_counters");
  uint n=0;
  while ((table= li++))
  {
    NESTED_JOIN *nested_join;
    bool is_eliminated_nest= FALSE;
    if ((nested_join= table->nested_join))
    {
      nested_join->counter= 0;
      nested_join->n_tables= reset_nj_counters(join, &nested_join->join_list);
      if (!nested_join->n_tables)
        is_eliminated_nest= TRUE;
    }
    const table_map removed_tables= join->eliminated_tables |
                                    join->const_table_map;

    if ((table->nested_join && !is_eliminated_nest) ||
        (!table->nested_join && (table->table->map & ~removed_tables)))
      n++;
  }
  DBUG_RETURN(n);
}