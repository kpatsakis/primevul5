Item *st_select_lex::build_cond_for_grouping_fields(THD *thd, Item *cond,
						    bool no_top_clones)
{
  if (cond->get_extraction_flag() == FULL_EXTRACTION_FL)
  {
    if (no_top_clones)
      return cond;
    cond->clear_extraction_flag();
    return cond->build_clone(thd, thd->mem_root);
  }
  if (cond->type() == Item::COND_ITEM)
  {
    bool cond_and= false;
    Item_cond *new_cond;
    if (((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC)
    {
      cond_and= true;
      new_cond=  new (thd->mem_root) Item_cond_and(thd);
    }
    else
      new_cond= new (thd->mem_root) Item_cond_or(thd);
    if (!new_cond)
      return 0;		
    List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
    Item *item;
    while ((item=li++))
    {
      if (item->get_extraction_flag() == NO_EXTRACTION_FL)
      {
	DBUG_ASSERT(cond_and);
	item->clear_extraction_flag();
	continue;
      }
      Item *fix= build_cond_for_grouping_fields(thd, item,
						no_top_clones & cond_and);
      if (!fix)
      {
	if (cond_and)
	  continue;
	break;
      }
      new_cond->argument_list()->push_back(fix, thd->mem_root);
    }
    
    if (!cond_and && item)
    {
      while((item= li++))
	item->clear_extraction_flag();
      return 0;
    }
    switch (new_cond->argument_list()->elements) 
    {
    case 0:
      return 0;			
    case 1:
      return new_cond->argument_list()->head();
    default:
      return new_cond;
    }
  }
  return 0;
}