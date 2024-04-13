void make_leaves_list(THD *thd, List<TABLE_LIST> &list, TABLE_LIST *tables,
                      bool full_table_list, TABLE_LIST *boundary)
 
{
  for (TABLE_LIST *table= tables; table; table= table->next_local)
  {
    if (table == boundary)
      full_table_list= !full_table_list;
    if (full_table_list && table->is_merged_derived())
    {
      SELECT_LEX *select_lex= table->get_single_select();
      /*
        It's safe to use select_lex->leaf_tables because all derived
        tables/views were already prepared and has their leaf_tables
        set properly.
      */
      make_leaves_list(thd, list, select_lex->get_table_list(),
      full_table_list, boundary);
    }
    else
    {
      list.push_back(table, thd->mem_root);
    }
  }
}