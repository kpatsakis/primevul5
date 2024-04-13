static bool setup_natural_join_row_types(THD *thd,
                                         List<TABLE_LIST> *from_clause,
                                         Name_resolution_context *context)
{
  DBUG_ENTER("setup_natural_join_row_types");
  thd->where= "from clause";
  if (from_clause->elements == 0)
    DBUG_RETURN(false); /* We come here in the case of UNIONs. */

  /* 
     Do not redo work if already done:
     1) for stored procedures,
     2) for multitable update after lock failure and table reopening.
  */
  if (!context->select_lex->first_natural_join_processing)
  {
    context->first_name_resolution_table= context->natural_join_first_table;
    DBUG_PRINT("info", ("using cached setup_natural_join_row_types"));
    DBUG_RETURN(false);
  }

  List_iterator_fast<TABLE_LIST> table_ref_it(*from_clause);
  TABLE_LIST *table_ref; /* Current table reference. */
  /* Table reference to the left of the current. */
  TABLE_LIST *left_neighbor;
  /* Table reference to the right of the current. */
  TABLE_LIST *right_neighbor= NULL;

  /* Note that tables in the list are in reversed order */
  for (left_neighbor= table_ref_it++; left_neighbor ; )
  {
    table_ref= left_neighbor;
    do
    {
      left_neighbor= table_ref_it++;
    }
    while (left_neighbor && left_neighbor->sj_subq_pred);

    if (store_top_level_join_columns(thd, table_ref,
                                     left_neighbor, right_neighbor))
      DBUG_RETURN(true);
    if (left_neighbor)
    {
      TABLE_LIST *first_leaf_on_the_right;
      first_leaf_on_the_right= table_ref->first_leaf_for_name_resolution();
      left_neighbor->next_name_resolution_table= first_leaf_on_the_right;
    }
    right_neighbor= table_ref;
  }

  /*
    Store the top-most, left-most NATURAL/USING join, so that we start
    the search from that one instead of context->table_list. At this point
    right_neighbor points to the left-most top-level table reference in the
    FROM clause.
  */
  DBUG_ASSERT(right_neighbor);
  context->first_name_resolution_table=
    right_neighbor->first_leaf_for_name_resolution();
  /*
    This is only to ensure that first_name_resolution_table doesn't
    change on re-execution
  */
  context->natural_join_first_table= context->first_name_resolution_table;
  context->select_lex->first_natural_join_processing= false;
  DBUG_RETURN (false);
}