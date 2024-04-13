join_read_key(JOIN_TAB *tab)
{
  return join_read_key2(tab->join->thd, tab, tab->table, &tab->ref);
}