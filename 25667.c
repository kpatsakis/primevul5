Item_func_trig_cond::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                                    uint *and_level, table_map usable_tables,
                                    SARGABLE_PARAM **sargables)
{
  /* 
    Subquery optimization: Conditions that are pushed down into subqueries
    are wrapped into Item_func_trig_cond. We process the wrapped condition
    but need to set cond_guard for KEYUSE elements generated from it.
  */
  if (!join->group_list && !join->order &&
      join->unit->item && 
      join->unit->item->substype() == Item_subselect::IN_SUBS &&
      !join->unit->is_union())
  {
    KEY_FIELD *save= *key_fields;
    args[0]->add_key_fields(join, key_fields, and_level, usable_tables,
                            sargables);
    // Indicate that this ref access candidate is for subquery lookup:
    for (; save != *key_fields; save++)
      save->cond_guard= get_trig_var();
  }
}