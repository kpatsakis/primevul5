add_found_match_trig_cond(THD *thd, JOIN_TAB *tab, COND *cond,
                          JOIN_TAB *root_tab)
{
  COND *tmp;
  DBUG_ASSERT(cond != 0);
  if (tab == root_tab)
    return cond;
  if ((tmp= add_found_match_trig_cond(thd, tab->first_upper, cond, root_tab)))
    tmp= new (thd->mem_root) Item_func_trig_cond(thd, tmp, &tab->found);
  if (tmp)
  {
    tmp->quick_fix_field();
    tmp->update_used_tables();
  }
  return tmp;
}