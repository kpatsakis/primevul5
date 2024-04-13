int multi_update::prepare2(JOIN *join)
{
  if (!join->need_tmp || !join->tmp_table_keep_current_rowid)
    return 0;

  // there cannot be many tmp tables in multi-update
  JOIN_TAB *tmptab= join->join_tab + join->exec_join_tab_cnt();

  for (Item **it= tmptab->tmp_table_param->items_to_copy; *it ; it++)
  {
    TABLE *tbl= item_rowid_table(*it);
    if (!tbl)
      continue;
    for (uint i= 0; i < table_count; i++)
    {
      for (Item **it2= tmp_table_param[i].items_to_copy; *it2; it2++)
      {
        if (item_rowid_table(*it2) != tbl)
          continue;
        Item *fld= new (thd->mem_root)
                        Item_field(thd, (*it)->get_tmp_table_field());
        if (!fld)
          return 1;
        fld->set_result_field((*it2)->get_tmp_table_field());
        *it2= fld;
      }
    }
  }
  return 0;
}