bool st_select_lex::is_merged_child_of(st_select_lex *ancestor)
{
  bool all_merged= TRUE;
  for (SELECT_LEX *sl= this; sl && sl!=ancestor;
       sl=sl->outer_select())
  {
    Item *subs= sl->master_unit()->item;
    if (subs && subs->type() == Item::SUBSELECT_ITEM && 
        ((Item_subselect*)subs)->substype() == Item_subselect::IN_SUBS &&
        ((Item_in_subselect*)subs)->test_strategy(SUBS_SEMI_JOIN))
    {
      continue;
    }

    if (sl->master_unit()->derived &&
      sl->master_unit()->derived->is_merged_derived())
    {
      continue;
    }
    all_merged= FALSE;
    break;
  }
  return all_merged;
}