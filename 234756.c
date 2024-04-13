void propagate_new_equalities(THD *thd, Item *cond,
                              List<Item_equal> *new_equalities,
                              COND_EQUAL *inherited,
                              bool *is_simplifiable_cond)
{
  if (cond->type() == Item::COND_ITEM)
  {
    bool and_level= ((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC;
    if (and_level)
    {
      Item_cond_and *cond_and= (Item_cond_and *) cond; 
      List<Item_equal> *cond_equalities= &cond_and->m_cond_equal.current_level;
      cond_and->m_cond_equal.upper_levels= inherited;
      if (!cond_equalities->is_empty() && cond_equalities != new_equalities)
      {
        Item_equal *equal_item;
        List_iterator<Item_equal> it(*new_equalities);
	while ((equal_item= it++))
	{
          equal_item->merge_into_list(thd, cond_equalities, true, true);
        }
        List_iterator<Item_equal> ei(*cond_equalities);
        while ((equal_item= ei++))
	{
          if (equal_item->const_item() && !equal_item->val_int())
	  {
            *is_simplifiable_cond= true;
            return;
          }
        }
      }
    }

    Item *item;
    List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
    while ((item= li++))
    {
      COND_EQUAL *new_inherited= and_level && item->type() == Item::COND_ITEM ?
                                   &((Item_cond_and *) cond)->m_cond_equal :
                                   inherited;
      propagate_new_equalities(thd, item, new_equalities, new_inherited,
                               is_simplifiable_cond);
    }
  }
  else if (cond->type() == Item::FUNC_ITEM && 
           ((Item_cond*) cond)->functype() == Item_func::MULT_EQUAL_FUNC)
  {
    Item_equal *equal_item;
    List_iterator<Item_equal> it(*new_equalities);
    Item_equal *equality= (Item_equal *) cond;
    equality->upper_levels= inherited;
    while ((equal_item= it++))
    {
      equality->merge_with_check(thd, equal_item, true);
    }
    if (equality->const_item() && !equality->val_int())
      *is_simplifiable_cond= true;
  }
  else
  {
    cond= cond->propagate_equal_fields(thd,
                                       Item::Context_boolean(), inherited);
    cond->update_used_tables();
  }          
} 