st_select_lex::check_cond_extraction_for_grouping_fields(Item *cond,
                                                         TABLE_LIST *derived)
{
  cond->clear_extraction_flag();
  if (cond->type() == Item::COND_ITEM)
  {
    bool and_cond= ((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC;
    List<Item> *arg_list=  ((Item_cond*) cond)->argument_list();
    List_iterator<Item> li(*arg_list);
    uint count= 0;         // to count items not containing NO_EXTRACTION_FL
    uint count_full= 0;    // to count items with FULL_EXTRACTION_FL
    Item *item;
    while ((item=li++))
    {
      check_cond_extraction_for_grouping_fields(item, derived);
      if (item->get_extraction_flag() !=  NO_EXTRACTION_FL)
      {
        count++;
        if (item->get_extraction_flag() == FULL_EXTRACTION_FL)
          count_full++;
      }
      else if (!and_cond)
        break;
    }
    if ((and_cond && count == 0) || item)
      cond->set_extraction_flag(NO_EXTRACTION_FL);
    if (count_full == arg_list->elements)
      cond->set_extraction_flag(FULL_EXTRACTION_FL);
    if (cond->get_extraction_flag() != 0)
    {
      li.rewind();
      while ((item=li++))
        item->clear_extraction_flag();
    }
  }
  else
  {
    int fl= cond->excl_dep_on_grouping_fields(this) ?
      FULL_EXTRACTION_FL : NO_EXTRACTION_FL;
    cond->set_extraction_flag(fl);
  }
}