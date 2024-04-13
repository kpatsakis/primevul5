bool setup_tables_and_check_access(THD *thd, Name_resolution_context *context,
                                   List<TABLE_LIST> *from_clause,
                                   TABLE_LIST *tables, List<TABLE_LIST> &leaves,
                                   bool select_insert, ulong want_access_first,
                                   ulong want_access, bool full_table_list)
{
  DBUG_ENTER("setup_tables_and_check_access");

  if (setup_tables(thd, context, from_clause, tables,
                   leaves, select_insert, full_table_list))
    DBUG_RETURN(TRUE);

  List_iterator<TABLE_LIST> ti(leaves);
  TABLE_LIST *table_list;
  ulong access= want_access_first;
  while ((table_list= ti++))
  {
    if (table_list->belong_to_view && !table_list->view && 
        check_single_table_access(thd, access, table_list, FALSE))
    {
      tables->hide_view_error(thd);
      DBUG_RETURN(TRUE);
    }
    access= want_access;
  }
  DBUG_RETURN(FALSE);
}