Item_equal::excl_dep_on_grouping_fields(st_select_lex *sel)
{
  Item_equal_fields_iterator it(*this);
  Item *item;

  while ((item=it++))
  {
    if (item->excl_dep_on_grouping_fields(sel))
    {
      set_extraction_flag(FULL_EXTRACTION_FL);
      return true;
    }
  }
  return false;
}