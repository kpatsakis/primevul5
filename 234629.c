static void print_table_array(THD *thd, 
                              table_map eliminated_tables,
                              String *str, TABLE_LIST **table, 
                              TABLE_LIST **end,
                              enum_query_type query_type)
{
  (*table)->print(thd, eliminated_tables, str, query_type);

  for (TABLE_LIST **tbl= table + 1; tbl < end; tbl++)
  {
    TABLE_LIST *curr= *tbl;
    
    /*
      The "eliminated_tables &&" check guards againist the case of 
      printing the query for CREATE VIEW. We do that without having run 
      JOIN::optimize() and so will have nested_join->used_tables==0.
    */
    if (eliminated_tables &&
        ((curr->table && (curr->table->map & eliminated_tables)) ||
         (curr->nested_join && !(curr->nested_join->used_tables &
                                ~eliminated_tables))))
    {
      /* as of 5.5, print_join doesnt put eliminated elements into array */
      DBUG_ASSERT(0); 
      continue;
    }

    /* JOIN_TYPE_OUTER is just a marker unrelated to real join */
    if (curr->outer_join & (JOIN_TYPE_LEFT|JOIN_TYPE_RIGHT))
    {
      /* MySQL converts right to left joins */
      str->append(STRING_WITH_LEN(" left join "));
    }
    else if (curr->straight)
      str->append(STRING_WITH_LEN(" straight_join "));
    else if (curr->sj_inner_tables)
      str->append(STRING_WITH_LEN(" semi join "));
    else
      str->append(STRING_WITH_LEN(" join "));
    
    curr->print(thd, eliminated_tables, str, query_type);
    if (curr->on_expr)
    {
      str->append(STRING_WITH_LEN(" on("));
      curr->on_expr->print(str, query_type);
      str->append(')');
    }
  }
}