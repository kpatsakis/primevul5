JOIN_TAB *first_top_level_tab(JOIN *join, enum enum_with_const_tables const_tbls)
{
  JOIN_TAB *tab= join->join_tab;
  if (const_tbls == WITHOUT_CONST_TABLES)
  {
    if (join->const_tables == join->table_count || !tab)
      return NULL;
    tab += join->const_tables;
  }
  return tab;
}