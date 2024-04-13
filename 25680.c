static COND* substitute_for_best_equal_field(THD *thd, JOIN_TAB *context_tab,
                                             COND *cond,
                                             COND_EQUAL *cond_equal,
                                             void *table_join_idx)
{
  Item_equal *item_equal;
  COND *org_cond= cond;                 // Return this in case of fatal error

  if (cond->type() == Item::COND_ITEM)
  {
    List<Item> *cond_list= ((Item_cond*) cond)->argument_list();

    bool and_level= ((Item_cond*) cond)->functype() ==
                      Item_func::COND_AND_FUNC;
    if (and_level)
    {
      cond_equal= &((Item_cond_and *) cond)->m_cond_equal;
      cond_list->disjoin((List<Item> *) &cond_equal->current_level);/* remove Item_equal objects from the AND. */

      List_iterator_fast<Item_equal> it(cond_equal->current_level);      
      while ((item_equal= it++))
      {
        item_equal->sort(&compare_fields_by_table_order, table_join_idx);
      }
    }
    
    List_iterator<Item> li(*cond_list);
    Item *item;
    while ((item= li++))
    {
      Item *new_item= substitute_for_best_equal_field(thd, context_tab,
                                                      item, cond_equal,
                                                      table_join_idx);
      /*
        This works OK with PS/SP re-execution as changes are made to
        the arguments of AND/OR items only
      */
      if (new_item != item)
        li.replace(new_item);
    }

    if (and_level)
    {
      COND *eq_cond= 0;
      List_iterator_fast<Item_equal> it(cond_equal->current_level);
      bool false_eq_cond= FALSE;
      while ((item_equal= it++))
      {
        eq_cond= eliminate_item_equal(thd, eq_cond, cond_equal->upper_levels,
                                      item_equal);
        if (!eq_cond)
	{
          eq_cond= 0;
          break;
        }
        else if (eq_cond->type() == Item::INT_ITEM && !eq_cond->val_bool()) 
	{
          /*
            This occurs when eliminate_item_equal() founds that cond is
            always false and substitutes it with Item_int 0.
            Due to this, value of item_equal will be 0, so just return it.
	  */
          cond= eq_cond;
          false_eq_cond= TRUE;
          break;
        }
      }
      if (eq_cond && !false_eq_cond)
      {
        /* Insert the generated equalities before all other conditions */
        if (eq_cond->type() == Item::COND_ITEM)
          ((Item_cond *) cond)->add_at_head(
                                  ((Item_cond *) eq_cond)->argument_list());
        else
	{
          if (cond_list->is_empty())
            cond= eq_cond;
          else
	  {
             /* Do not add an equality condition if it's always true */ 
             if (eq_cond->type() != Item::INT_ITEM &&
                 cond_list->push_front(eq_cond, thd->mem_root))
               eq_cond= 0;
          }
	}
      }
      if (!eq_cond)
      {
        /* 
          We are out of memory doing the transformation.
          This is a fatal error now. However we bail out by returning the
          original condition that we had before we started the transformation. 
	*/
	cond_list->append((List<Item> *) &cond_equal->current_level);
      }
    }	 
  }
  else if (cond->type() == Item::FUNC_ITEM && 
           ((Item_func*) cond)->functype() == Item_func::MULT_EQUAL_FUNC)
  {
    item_equal= (Item_equal *) cond;
    item_equal->sort(&compare_fields_by_table_order, table_join_idx);
    cond_equal= item_equal->upper_levels;
    if (cond_equal && cond_equal->current_level.head() == item_equal)
      cond_equal= cond_equal->upper_levels;
    cond= eliminate_item_equal(thd, 0, cond_equal, item_equal);
    return cond ? cond : org_cond;
  }
  else 
  {
    while (cond_equal)
    {
      List_iterator_fast<Item_equal> it(cond_equal->current_level);
      while((item_equal= it++))
      {
        REPLACE_EQUAL_FIELD_ARG arg= {item_equal, context_tab};
        cond= cond->transform(thd, &Item::replace_equal_field, (uchar *) &arg);
      }
      cond_equal= cond_equal->upper_levels;
    }
  }
  return cond;
}