bool multi_update::init(THD *thd)
{
  table_map tables_to_update= get_table_map(fields);
  List_iterator_fast<TABLE_LIST> li(*leaves);
  TABLE_LIST *tbl;
  while ((tbl =li++))
  {
    if (tbl->is_jtbm())
      continue;
    if (!(tbl->table->map & tables_to_update))
      continue;
    if (updated_leaves.push_back(tbl, thd->mem_root))
      return true;
  }
  return false;
}