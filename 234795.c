bool error_if_full_join(JOIN *join)
{
  for (JOIN_TAB *tab=first_top_level_tab(join, WITH_CONST_TABLES); tab;
       tab= next_top_level_tab(join, tab))
  {
    if (tab->type == JT_ALL && (!tab->select || !tab->select->quick))
    {
      my_message(ER_UPDATE_WITHOUT_KEY_IN_SAFE_MODE,
                 ER_THD(join->thd,
                        ER_UPDATE_WITHOUT_KEY_IN_SAFE_MODE), MYF(0));
      return(1);
    }
  }
  return(0);
}