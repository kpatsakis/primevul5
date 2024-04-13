bool st_select_lex::get_free_table_map(table_map *map, uint *tablenr)
{
  *map= 0;
  *tablenr= 0;
  TABLE_LIST *tl;
  List_iterator<TABLE_LIST> ti(leaf_tables);
  while ((tl= ti++))
  {
    if (tl->table->map > *map)
      *map= tl->table->map;
    if (tl->table->tablenr > *tablenr)
      *tablenr= tl->table->tablenr;
  }
  (*map)<<= 1;
  (*tablenr)++;
  if (*tablenr >= MAX_TABLES)
    return TRUE;
  return FALSE;
}