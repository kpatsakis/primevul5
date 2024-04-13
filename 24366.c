void st_select_lex::append_table_to_list(TABLE_LIST *TABLE_LIST::*link,
                                         TABLE_LIST *table)
{
  TABLE_LIST *tl;
  for (tl= leaf_tables.head(); tl->*link; tl= tl->*link) ;
  tl->*link= table;
}