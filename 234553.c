bool JOIN_TAB::keyuse_is_valid_for_access_in_chosen_plan(JOIN *join,
                                                         KEYUSE *keyuse)
{
  if (!access_from_tables_is_allowed(keyuse->used_tables, 
                                     join->sjm_lookup_tables))
    return false;
  if (join->sjm_scan_tables & table->map)
    return true;
  table_map keyuse_sjm_scan_tables= keyuse->used_tables &
                                    join->sjm_scan_tables;
  if (!keyuse_sjm_scan_tables)
    return true;
  uint sjm_tab_nr= 0;
  while (!(keyuse_sjm_scan_tables & table_map(1) << sjm_tab_nr))
    sjm_tab_nr++;
  JOIN_TAB *sjm_tab= join->map2table[sjm_tab_nr];
  TABLE_LIST *emb_sj_nest= sjm_tab->emb_sj_nest;    
  if (!(emb_sj_nest->sj_mat_info && emb_sj_nest->sj_mat_info->is_used &&
        emb_sj_nest->sj_mat_info->is_sj_scan))
    return true;
  st_select_lex *sjm_sel= emb_sj_nest->sj_subq_pred->unit->first_select(); 
  for (uint i= 0; i < sjm_sel->item_list.elements; i++)
  {
    DBUG_ASSERT(sjm_sel->ref_pointer_array[i]->real_item()->type() == Item::FIELD_ITEM);
    if (keyuse->val->real_item()->type() == Item::FIELD_ITEM)
    {
      Field *field = ((Item_field*)sjm_sel->ref_pointer_array[i]->real_item())->field;
      if (field->eq(((Item_field*)keyuse->val->real_item())->field))
        return true;
    }
  }
  return false; 
}