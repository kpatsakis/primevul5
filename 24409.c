void st_select_lex::replace_leaf_table(TABLE_LIST *table, List<TABLE_LIST> &tbl_list)
{
  TABLE_LIST *tl;
  List_iterator<TABLE_LIST> ti(leaf_tables);
  while ((tl= ti++))
  {
    if (tl == table)
    {
      ti.replace(tbl_list);
      break;
    }
  }
}