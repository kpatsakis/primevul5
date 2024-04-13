void LEX::link_first_table_back(TABLE_LIST *first,
				   bool link_to_local)
{
  if (first)
  {
    if ((first->next_global= query_tables))
      query_tables->prev_global= &first->next_global;
    else
      query_tables_last= &first->next_global;
    query_tables= first;

    if (link_to_local)
    {
      first->next_local= select_lex.table_list.first;
      select_lex.context.table_list= first;
      select_lex.table_list.first= first;
      select_lex.table_list.elements++;	//safety
    }
  }
}