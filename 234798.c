make_outerjoin_info(JOIN *join)
{
  DBUG_ENTER("make_outerjoin_info");
  
  /*
    Create temp. tables for merged SJ-Materialization nests. We need to do
    this now, because further code relies on tab->table and
    tab->table->pos_in_table_list being set.
  */
  JOIN_TAB *tab;
  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    if (tab->bush_children)
    {
      if (setup_sj_materialization_part1(tab))
        DBUG_RETURN(TRUE);
      tab->table->reginfo.join_tab= tab;
    }
  }

  for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
  {
    TABLE *table= tab->table;
    TABLE_LIST *tbl= table->pos_in_table_list;
    TABLE_LIST *embedding= tbl->embedding;

    if (tbl->outer_join & (JOIN_TYPE_LEFT | JOIN_TYPE_RIGHT))
    {
      /* 
        Table tab is the only one inner table for outer join.
        (Like table t4 for the table reference t3 LEFT JOIN t4 ON t3.a=t4.a
        is in the query above.)
      */
      tab->last_inner= tab->first_inner= tab;
      tab->on_expr_ref= &tbl->on_expr;
      tab->cond_equal= tbl->cond_equal;
      if (embedding && !embedding->is_active_sjm())
        tab->first_upper= embedding->nested_join->first_nested;
    }
    else if (!embedding)
      tab->table->reginfo.not_exists_optimize= 0;
          
    for ( ; embedding ; embedding= embedding->embedding)
    {
      if (embedding->is_active_sjm())
      {
        /* We're trying to walk out of an SJ-Materialization nest. Don't do this.  */
        break;
      }
      /* Ignore sj-nests: */
      if (!(embedding->on_expr && embedding->outer_join))
      {
        tab->table->reginfo.not_exists_optimize= 0;
        continue;
      }
      NESTED_JOIN *nested_join= embedding->nested_join;
      if (!nested_join->counter)
      {
        /* 
          Table tab is the first inner table for nested_join.
          Save reference to it in the nested join structure.
        */ 
        nested_join->first_nested= tab;
        tab->on_expr_ref= &embedding->on_expr;
        tab->cond_equal= tbl->cond_equal;
        if (embedding->embedding)
          tab->first_upper= embedding->embedding->nested_join->first_nested;
      }
      if (!tab->first_inner)  
        tab->first_inner= nested_join->first_nested;
      if (++nested_join->counter < nested_join->n_tables)
        break;
      /* Table tab is the last inner table for nested join. */
      nested_join->first_nested->last_inner= tab;
    }
  }
  DBUG_RETURN(FALSE);
}