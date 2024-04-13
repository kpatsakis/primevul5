Item *remove_pushed_top_conjuncts(THD *thd, Item *cond)
{
  if (cond->get_extraction_flag() == FULL_EXTRACTION_FL)
  {
    cond->clear_extraction_flag();
    return 0; 
  }
  if (cond->type() == Item::COND_ITEM)
  {
    if (((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC)
    {
      List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
      Item *item;
      while ((item= li++))
      {
	if (item->get_extraction_flag() == FULL_EXTRACTION_FL)
	{
	  item->clear_extraction_flag();
	  li.remove();
	}
      }
      switch (((Item_cond*) cond)->argument_list()->elements) 
      {
      case 0:
	return 0;			
      case 1:
	return ((Item_cond*) cond)->argument_list()->head();
      default:
	return cond;
      }
    }
  }
  return cond;
}