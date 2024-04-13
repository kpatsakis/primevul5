static bool is_eliminated_table(table_map eliminated_tables, TABLE_LIST *tbl)
{
  return eliminated_tables &&
    ((tbl->table && (tbl->table->map & eliminated_tables)) ||
     (tbl->nested_join && !(tbl->nested_join->used_tables &
                            ~eliminated_tables)));
}