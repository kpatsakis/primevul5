bool setup_tables(THD *thd, Name_resolution_context *context,
                  List<TABLE_LIST> *from_clause, TABLE_LIST *tables,
                  List<TABLE_LIST> &leaves, bool select_insert,
                  bool full_table_list)
{
  uint tablenr= 0;
  List_iterator<TABLE_LIST> ti(leaves);
  TABLE_LIST *table_list;

  DBUG_ENTER("setup_tables");

  DBUG_ASSERT ((select_insert && !tables->next_name_resolution_table) || !tables || 
               (context->table_list && context->first_name_resolution_table));
  /*
    this is used for INSERT ... SELECT.
    For select we setup tables except first (and its underlying tables)
  */
  TABLE_LIST *first_select_table= (select_insert ?
                                   tables->next_local:
                                   0);
  SELECT_LEX *select_lex= select_insert ? thd->lex->first_select_lex() :
                                          thd->lex->current_select;
  if (select_lex->first_cond_optimization)
  {
    leaves.empty();
    if (select_lex->prep_leaf_list_state != SELECT_LEX::SAVED)
    {
      make_leaves_list(thd, leaves, tables, full_table_list, first_select_table);
      select_lex->prep_leaf_list_state= SELECT_LEX::READY;
      select_lex->leaf_tables_exec.empty();
    }
    else
    {
      List_iterator_fast <TABLE_LIST> ti(select_lex->leaf_tables_prep);
      while ((table_list= ti++))
        leaves.push_back(table_list, thd->mem_root);
    }
      
    while ((table_list= ti++))
    {
      TABLE *table= table_list->table;
      if (table)
        table->pos_in_table_list= table_list;
      if (first_select_table &&
          table_list->top_table() == first_select_table)
      {
        /* new counting for SELECT of INSERT ... SELECT command */
        first_select_table= 0;
        thd->lex->first_select_lex()->insert_tables= tablenr;
        tablenr= 0;
      }
      if(table_list->jtbm_subselect)
      {
        table_list->jtbm_table_no= tablenr;
      }
      else if (table)
      {
        table->pos_in_table_list= table_list;
        setup_table_map(table, table_list, tablenr);

        if (table_list->process_index_hints(table))
          DBUG_RETURN(1);
      }
      tablenr++;
      /*
        We test the max tables here as we setup_table_map() should not be called
        with tablenr >= 64
      */
      if (tablenr > MAX_TABLES)
      {
        my_error(ER_TOO_MANY_TABLES,MYF(0), static_cast<int>(MAX_TABLES));
        DBUG_RETURN(1);
      }
    }
  }
  else
  { 
    List_iterator_fast <TABLE_LIST> ti(select_lex->leaf_tables_exec);
    select_lex->leaf_tables.empty();
    while ((table_list= ti++))
    {
      if(table_list->jtbm_subselect)
      {
        table_list->jtbm_table_no= table_list->tablenr_exec;
      }
      else
      {
        table_list->table->tablenr= table_list->tablenr_exec;
        table_list->table->map= table_list->map_exec;
        table_list->table->maybe_null= table_list->maybe_null_exec;
        table_list->table->pos_in_table_list= table_list;
        if (table_list->process_index_hints(table_list->table))
          DBUG_RETURN(1);
      }
      select_lex->leaf_tables.push_back(table_list);
    }
  }    

  for (table_list= tables;
       table_list;
       table_list= table_list->next_local)
  {
    if (table_list->merge_underlying_list)
    {
      DBUG_ASSERT(table_list->is_merged_derived());
      Query_arena *arena, backup;
      arena= thd->activate_stmt_arena_if_needed(&backup);
      bool res;
      res= table_list->setup_underlying(thd);
      if (arena)
        thd->restore_active_arena(arena, &backup);
      if (res)
        DBUG_RETURN(1);
    }

    if (table_list->jtbm_subselect)
    {
      Item *item= table_list->jtbm_subselect->optimizer;
      if (!table_list->jtbm_subselect->optimizer->fixed &&
          table_list->jtbm_subselect->optimizer->fix_fields(thd, &item))
      {
        my_error(ER_TOO_MANY_TABLES,MYF(0), static_cast<int>(MAX_TABLES)); /* psergey-todo: WHY ER_TOO_MANY_TABLES ???*/
        DBUG_RETURN(1);
      }
      DBUG_ASSERT(item == table_list->jtbm_subselect->optimizer);
    }
  }

  /* Precompute and store the row types of NATURAL/USING joins. */
  if (setup_natural_join_row_types(thd, from_clause, context))
    DBUG_RETURN(1);

  DBUG_RETURN(0);
}