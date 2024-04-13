void mark_join_nest_as_const(JOIN *join,
                             TABLE_LIST *join_nest,
                             table_map *found_const_table_map,
                             uint *const_count)
{
  List_iterator<TABLE_LIST> it(join_nest->nested_join->join_list);
  TABLE_LIST *tbl;
  while ((tbl= it++))
  {
    if (tbl->nested_join)
    {
      mark_join_nest_as_const(join, tbl, found_const_table_map, const_count);
      continue;
    }
    JOIN_TAB *tab= tbl->table->reginfo.join_tab;

    if (!(join->const_table_map & tab->table->map))
    {
      tab->type= JT_CONST;
      tab->info= ET_IMPOSSIBLE_ON_CONDITION;
      tab->table->const_table= 1;

      join->const_table_map|= tab->table->map;
      *found_const_table_map|= tab->table->map;
      set_position(join,(*const_count)++,tab,(KEYUSE*) 0);
      mark_as_null_row(tab->table);		// All fields are NULL
    }
  }
}