static uint build_bitmap_for_nested_joins(List<TABLE_LIST> *join_list, 
                                          uint first_unused)
{
  List_iterator<TABLE_LIST> li(*join_list);
  TABLE_LIST *table;
  DBUG_ENTER("build_bitmap_for_nested_joins");
  while ((table= li++))
  {
    NESTED_JOIN *nested_join;
    if ((nested_join= table->nested_join))
    {
      /*
        It is guaranteed by simplify_joins() function that a nested join
        that has only one child represents a single table VIEW (and the child
        is an underlying table). We don't assign bits to such nested join
        structures because 
        1. it is redundant (a "sequence" of one table cannot be interleaved 
            with anything)
        2. we could run out bits in nested_join_map otherwise.
      */
      if (nested_join->n_tables != 1)
      {
        /* Don't assign bits to sj-nests */
        if (table->on_expr)
          nested_join->nj_map= (nested_join_map) 1 << first_unused++;
        first_unused= build_bitmap_for_nested_joins(&nested_join->join_list,
                                                    first_unused);
      }
    }
  }
  DBUG_RETURN(first_unused);
}