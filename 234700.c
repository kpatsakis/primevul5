simplify_joins(JOIN *join, List<TABLE_LIST> *join_list, COND *conds, bool top,
               bool in_sj)
{
  TABLE_LIST *table;
  NESTED_JOIN *nested_join;
  TABLE_LIST *prev_table= 0;
  List_iterator<TABLE_LIST> li(*join_list);
  bool straight_join= MY_TEST(join->select_options & SELECT_STRAIGHT_JOIN);
  DBUG_ENTER("simplify_joins");

  /* 
    Try to simplify join operations from join_list.
    The most outer join operation is checked for conversion first. 
  */
  while ((table= li++))
  {
    table_map used_tables;
    table_map not_null_tables= (table_map) 0;

    if ((nested_join= table->nested_join))
    {
      /* 
         If the element of join_list is a nested join apply
         the procedure to its nested join list first.
      */
      if (table->on_expr)
      {
        Item *expr= table->on_expr;
        /* 
           If an on expression E is attached to the table, 
           check all null rejected predicates in this expression.
           If such a predicate over an attribute belonging to
           an inner table of an embedded outer join is found,
           the outer join is converted to an inner join and
           the corresponding on expression is added to E. 
	*/ 
        expr= simplify_joins(join, &nested_join->join_list,
                             expr, FALSE, in_sj || table->sj_on_expr);

        if (!table->prep_on_expr || expr != table->on_expr)
        {
          DBUG_ASSERT(expr);

          table->on_expr= expr;
          table->prep_on_expr= expr->copy_andor_structure(join->thd);
        }
      }
      nested_join->used_tables= (table_map) 0;
      nested_join->not_null_tables=(table_map) 0;
      conds= simplify_joins(join, &nested_join->join_list, conds, top, 
                            in_sj || table->sj_on_expr);
      used_tables= nested_join->used_tables;
      not_null_tables= nested_join->not_null_tables;  
      /* The following two might become unequal after table elimination: */
      nested_join->n_tables= nested_join->join_list.elements;
    }
    else
    {
      if (!table->prep_on_expr)
        table->prep_on_expr= table->on_expr;
      used_tables= table->get_map();
      if (conds)
        not_null_tables= conds->not_null_tables();
    }
      
    if (table->embedding)
    {
      table->embedding->nested_join->used_tables|= used_tables;
      table->embedding->nested_join->not_null_tables|= not_null_tables;
    }

    if (!(table->outer_join & (JOIN_TYPE_LEFT | JOIN_TYPE_RIGHT)) ||
        (used_tables & not_null_tables))
    {
      /* 
        For some of the inner tables there are conjunctive predicates
        that reject nulls => the outer join can be replaced by an inner join.
      */
      if (table->outer_join && !table->embedding && table->table)
        table->table->maybe_null= FALSE;
      table->outer_join= 0;
      if (!(straight_join || table->straight))
      {
        table->dep_tables= 0;
        TABLE_LIST *embedding= table->embedding;
        while (embedding)
        {
          if (embedding->nested_join->join_list.head()->outer_join)
          {
            if (!embedding->sj_subq_pred)
              table->dep_tables= embedding->dep_tables;
            break;
          }
          embedding= embedding->embedding;
        }
      }
      if (table->on_expr)
      {
        /* Add ON expression to the WHERE or upper-level ON condition. */
        if (conds)
        {
          conds= and_conds(join->thd, conds, table->on_expr);
          conds->top_level_item();
          /* conds is always a new item as both cond and on_expr existed */
          DBUG_ASSERT(!conds->fixed);
          conds->fix_fields(join->thd, &conds);
        }
        else
          conds= table->on_expr; 
        table->prep_on_expr= table->on_expr= 0;
      }
    }

    /* 
      Only inner tables of non-convertible outer joins
      remain with on_expr.
    */ 
    if (table->on_expr)
    {
      table_map table_on_expr_used_tables= table->on_expr->used_tables();
      table->dep_tables|= table_on_expr_used_tables;
      if (table->embedding)
      {
        table->dep_tables&= ~table->embedding->nested_join->used_tables;   
        /*
           Embedding table depends on tables used
           in embedded on expressions. 
        */
        table->embedding->on_expr_dep_tables|= table_on_expr_used_tables;
      }
      else
        table->dep_tables&= ~table->get_map();
    }

    if (prev_table)
    {
      /* The order of tables is reverse: prev_table follows table */
      if (prev_table->straight || straight_join)
        prev_table->dep_tables|= used_tables;
      if (prev_table->on_expr)
      {
        prev_table->dep_tables|= table->on_expr_dep_tables;
        table_map prev_used_tables= prev_table->nested_join ?
	                            prev_table->nested_join->used_tables :
	                            prev_table->get_map();
        /* 
          If on expression contains only references to inner tables
          we still make the inner tables dependent on the outer tables.
          It would be enough to set dependency only on one outer table
          for them. Yet this is really a rare case.
          Note:
          RAND_TABLE_BIT mask should not be counted as it
          prevents update of inner table dependences.
          For example it might happen if RAND() function
          is used in JOIN ON clause.
	*/  
        if (!((prev_table->on_expr->used_tables() &
               ~(OUTER_REF_TABLE_BIT | RAND_TABLE_BIT)) &
              ~prev_used_tables))
          prev_table->dep_tables|= used_tables;
      }
    }
    prev_table= table;
  }
    
  /* 
    Flatten nested joins that can be flattened.
    no ON expression and not a semi-join => can be flattened.
  */
  li.rewind();
  while ((table= li++))
  {
    nested_join= table->nested_join;
    if (table->sj_on_expr && !in_sj)
    {
      /*
        If this is a semi-join that is not contained within another semi-join
        leave it intact (otherwise it is flattened)
      */
      /*
        Make sure that any semi-join appear in
        the join->select_lex->sj_nests list only once
      */
      List_iterator_fast<TABLE_LIST> sj_it(join->select_lex->sj_nests);
      TABLE_LIST *sj_nest;
      while ((sj_nest= sj_it++))
      {
        if (table == sj_nest)
          break;
      }
      if (sj_nest)
        continue;
      join->select_lex->sj_nests.push_back(table, join->thd->mem_root);

      /* 
        Also, walk through semi-join children and mark those that are now
        top-level
      */
      TABLE_LIST *tbl;
      List_iterator<TABLE_LIST> it(nested_join->join_list);
      while ((tbl= it++))
      {
        if (!tbl->on_expr && tbl->table)
          tbl->table->maybe_null= FALSE;
      }
    }
    else if (nested_join && !table->on_expr)
    {
      TABLE_LIST *tbl;
      List_iterator<TABLE_LIST> it(nested_join->join_list);
      List<TABLE_LIST> repl_list;  
      while ((tbl= it++))
      {
        tbl->embedding= table->embedding;
        if (!tbl->embedding && !tbl->on_expr && tbl->table)
          tbl->table->maybe_null= FALSE;
        tbl->join_list= table->join_list;
        repl_list.push_back(tbl, join->thd->mem_root);
        tbl->dep_tables|= table->dep_tables;
      }
      li.replace(repl_list);
    }
  }
  DBUG_RETURN(conds); 
}