make_cond_after_sjm(THD *thd, Item *root_cond, Item *cond, table_map tables,
                    table_map sjm_tables, bool inside_or_clause)
{
  /*
    We assume that conditions that refer to only join prefix tables or 
    sjm_tables have already been checked.
  */
  if (!inside_or_clause)
  {
    table_map cond_used_tables= cond->used_tables();
    if((!(cond_used_tables & ~tables) ||
       !(cond_used_tables & ~sjm_tables)))
      return (COND*) 0;				// Already checked
  }

  /* AND/OR recursive descent */
  if (cond->type() == Item::COND_ITEM)
  {
    if (((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC)
    {
      /* Create new top level AND item */
      Item_cond_and *new_cond= new (thd->mem_root) Item_cond_and(thd);
      if (!new_cond)
	return (COND*) 0;			// OOM /* purecov: inspected */
      List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
      Item *item;
      while ((item=li++))
      {
        Item *fix=make_cond_after_sjm(thd, root_cond, item, tables, sjm_tables,
                                      inside_or_clause);
	if (fix)
	  new_cond->argument_list()->push_back(fix, thd->mem_root);
      }
      switch (new_cond->argument_list()->elements) {
      case 0:
	return (COND*) 0;			// Always true
      case 1:
	return new_cond->argument_list()->head();
      default:
	/*
	  Item_cond_and do not need fix_fields for execution, its parameters
	  are fixed or do not need fix_fields, too
	*/
	new_cond->quick_fix_field();
	new_cond->used_tables_cache=
	  ((Item_cond_and*) cond)->used_tables_cache &
	  tables;
	return new_cond;
      }
    }
    else
    {						// Or list
      Item_cond_or *new_cond= new (thd->mem_root) Item_cond_or(thd);
      if (!new_cond)
	return (COND*) 0;			// OOM /* purecov: inspected */
      List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
      Item *item;
      while ((item=li++))
      {
        Item *fix= make_cond_after_sjm(thd, root_cond, item, tables, sjm_tables,
                                       /*inside_or_clause= */TRUE);
	if (!fix)
	  return (COND*) 0;			// Always true
	new_cond->argument_list()->push_back(fix, thd->mem_root);
      }
      /*
	Item_cond_or do not need fix_fields for execution, its parameters
	are fixed or do not need fix_fields, too
      */
      new_cond->quick_fix_field();
      new_cond->used_tables_cache= ((Item_cond_or*) cond)->used_tables_cache;
      new_cond->top_level_item();
      return new_cond;
    }
  }

  /*
    Because the following test takes a while and it can be done
    table_count times, we mark each item that we have examined with the result
    of the test
  */

  if (cond->marker == 3 || (cond->used_tables() & ~(tables | sjm_tables)))
    return (COND*) 0;				// Can't check this yet
  if (cond->marker == 2 || cond->eq_cmp_result() == Item::COND_OK)
    return cond;				// Not boolean op

  /* 
    Remove equalities that are guaranteed to be true by use of 'ref' access
    method
  */
  if (((Item_func*) cond)->functype() == Item_func::EQ_FUNC)
  {
    Item *left_item= ((Item_func*) cond)->arguments()[0]->real_item();
    Item *right_item= ((Item_func*) cond)->arguments()[1]->real_item();
    if (left_item->type() == Item::FIELD_ITEM &&
	test_if_ref(root_cond, (Item_field*) left_item,right_item))
    {
      cond->marker=3;			// Checked when read
      return (COND*) 0;
    }
    if (right_item->type() == Item::FIELD_ITEM &&
	test_if_ref(root_cond, (Item_field*) right_item,left_item))
    {
      cond->marker=3;			// Checked when read
      return (COND*) 0;
    }
  }
  cond->marker=2;
  return cond;
}