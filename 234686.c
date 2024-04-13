bool JOIN::add_fields_for_current_rowid(JOIN_TAB *cur, List<Item> *table_fields)
{
  /*
    this will not walk into semi-join materialization nests but this is ok
    because we will never need to save current rowids for those.
  */
  for (JOIN_TAB *tab=join_tab; tab < cur; tab++)
  {
    if (!tab->keep_current_rowid)
      continue;
    Item *item= new (thd->mem_root) Item_temptable_rowid(tab->table);
    item->fix_fields(thd, 0);
    table_fields->push_back(item, thd->mem_root);
    cur->tmp_table_param->func_count++;
  }
  return 0;
}