void JOIN_TAB::remove_redundant_bnl_scan_conds()
{
  if (!(select_cond && cache_select && cache &&
        (cache->get_join_alg() == JOIN_CACHE::BNL_JOIN_ALG ||
         cache->get_join_alg() == JOIN_CACHE::BNLH_JOIN_ALG)))
    return;

  /*
    select->cond is not processed separately. This method assumes it is always
    the same as select_cond.
  */
  if (select && select->cond != select_cond)
    return;

  if (is_cond_and(select_cond))
  {
    List_iterator<Item> pushed_cond_li(*((Item_cond*) select_cond)->argument_list());
    Item *pushed_item;
    Item_cond_and *reduced_select_cond= new (join->thd->mem_root)
      Item_cond_and(join->thd);

    if (is_cond_and(cache_select->cond))
    {
      List_iterator<Item> scan_cond_li(*((Item_cond*) cache_select->cond)->argument_list());
      Item *scan_item;
      while ((pushed_item= pushed_cond_li++))
      {
        bool found_cond= false;
        scan_cond_li.rewind();
        while ((scan_item= scan_cond_li++))
        {
          if (pushed_item->eq(scan_item, 0))
          {
            found_cond= true;
            break;
          }
        }
        if (!found_cond)
          reduced_select_cond->add(pushed_item, join->thd->mem_root);
      }
    }
    else
    {
      while ((pushed_item= pushed_cond_li++))
      {
        if (!pushed_item->eq(cache_select->cond, 0))
          reduced_select_cond->add(pushed_item, join->thd->mem_root);
      }
    }

    /*
      JOIN_CACHE::check_match uses JOIN_TAB::select->cond instead of
      JOIN_TAB::select_cond. set_cond() sets both pointers.
    */
    if (reduced_select_cond->argument_list()->is_empty())
      set_cond(NULL);
    else if (reduced_select_cond->argument_list()->elements == 1)
      set_cond(reduced_select_cond->argument_list()->head());
    else
    {
      reduced_select_cond->quick_fix_field();
      set_cond(reduced_select_cond);
    }
  }
  else if (select_cond->eq(cache_select->cond, 0))
    set_cond(NULL);
}