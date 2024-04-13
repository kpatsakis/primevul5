make_cond_for_table_from_pred(THD *thd, Item *root_cond, Item *cond,
                              table_map tables, table_map used_table,
                              int join_tab_idx_arg,
                              bool exclude_expensive_cond __attribute__
                              ((unused)),
                              bool retain_ref_cond,
                              bool is_top_and_level)

{
  table_map rand_table_bit= (table_map) RAND_TABLE_BIT;

  if (used_table && !(cond->used_tables() & used_table))
    return (COND*) 0;				// Already checked

  if (cond->type() == Item::COND_ITEM)
  {
    if (((Item_cond*) cond)->functype() == Item_func::COND_AND_FUNC)
    {
      /* Create new top level AND item */
      Item_cond_and *new_cond=new (thd->mem_root) Item_cond_and(thd);
      if (!new_cond)
	return (COND*) 0;			// OOM /* purecov: inspected */
      List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
      Item *item;
      while ((item=li++))
      {
        /*
          Special handling of top level conjuncts with RAND_TABLE_BIT:
          if such a conjunct contains a reference to a field that is not
          an outer field then it is pushed to the corresponding table by
          the same rule as all other conjuncts. Otherwise, if the conjunct
          is used in WHERE is is pushed to the last joined table, if is it
          is used in ON condition of an outer join it is pushed into the
          last inner table of the outer join. Such conjuncts are pushed in
          a call of make_cond_for_table_from_pred() with the
          parameter 'used_table' equal to PSEUDO_TABLE_BITS.
        */
        if (is_top_and_level && used_table == rand_table_bit &&
            (item->used_tables() & ~OUTER_REF_TABLE_BIT) != rand_table_bit)
        {
          /* The conjunct with RAND_TABLE_BIT has been allready pushed */
          continue;
        }
	Item *fix=make_cond_for_table_from_pred(thd, root_cond, item, 
                                                tables, used_table,
                                                join_tab_idx_arg,
                                                exclude_expensive_cond,
                                                retain_ref_cond, false);
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
          Call fix_fields to propagate all properties of the children to
          the new parent Item. This should not be expensive because all
	  children of Item_cond_and should be fixed by now.
	*/
	if (new_cond->fix_fields(thd, 0))
          return (COND*) 0;
	new_cond->used_tables_cache=
	  ((Item_cond_and*) cond)->used_tables_cache &
	  tables;
	return new_cond;
      }
    }
    else
    {						// Or list
      if (is_top_and_level && used_table == rand_table_bit &&
          (cond->used_tables() & ~OUTER_REF_TABLE_BIT) != rand_table_bit)
      {
        /* This top level formula with RAND_TABLE_BIT has been already pushed */
        return (COND*) 0;
      }

      Item_cond_or *new_cond=new (thd->mem_root) Item_cond_or(thd);
      if (!new_cond)
	return (COND*) 0;			// OOM /* purecov: inspected */
      List_iterator<Item> li(*((Item_cond*) cond)->argument_list());
      Item *item;
      while ((item=li++))
      {
	Item *fix=make_cond_for_table_from_pred(thd, root_cond, item,
                                                tables, 0L,
                                                join_tab_idx_arg,
                                                exclude_expensive_cond,
                                                retain_ref_cond, false);
	if (!fix)
	  return (COND*) 0;			// Always true
	new_cond->argument_list()->push_back(fix, thd->mem_root);
      }
      /*
        Call fix_fields to propagate all properties of the children to
        the new parent Item. This should not be expensive because all
        children of Item_cond_and should be fixed by now.
      */
      new_cond->fix_fields(thd, 0);
      new_cond->used_tables_cache= ((Item_cond_or*) cond)->used_tables_cache;
      new_cond->top_level_item();
      return new_cond;
    }
  }

  if (is_top_and_level && used_table == rand_table_bit &&
      (cond->used_tables() & ~OUTER_REF_TABLE_BIT) != rand_table_bit)
  {
    /* This top level formula with RAND_TABLE_BIT has been already pushed */
    return (COND*) 0;
  }

  /*
    Because the following test takes a while and it can be done
    table_count times, we mark each item that we have examined with the result
    of the test
  */
  if ((cond->marker == 3 && !retain_ref_cond) ||
      (cond->used_tables() & ~tables))
    return (COND*) 0;				// Can't check this yet

  if (cond->marker == 2 || cond->eq_cmp_result() == Item::COND_OK)
  {
    cond->set_join_tab_idx(join_tab_idx_arg);
    return cond;				// Not boolean op
  }

  if (cond->type() == Item::FUNC_ITEM && 
      ((Item_func*) cond)->functype() == Item_func::EQ_FUNC)
  {
    Item *left_item=	((Item_func*) cond)->arguments()[0]->real_item();
    Item *right_item= ((Item_func*) cond)->arguments()[1]->real_item();
    if (left_item->type() == Item::FIELD_ITEM && !retain_ref_cond &&
	test_if_ref(root_cond, (Item_field*) left_item,right_item))
    {
      cond->marker=3;			// Checked when read
      return (COND*) 0;
    }
    if (right_item->type() == Item::FIELD_ITEM && !retain_ref_cond &&
	test_if_ref(root_cond, (Item_field*) right_item,left_item))
    {
      cond->marker=3;			// Checked when read
      return (COND*) 0;
    }
    /*
      If cond is an equality injected for split optimization then
      a. when retain_ref_cond == false : cond is removed unconditionally
         (cond that supports ref access is removed by the preceding code)
      b. when retain_ref_cond == true : cond is removed if it does not
         support ref access
    */
    if (left_item->type() == Item::FIELD_ITEM &&
        is_eq_cond_injected_for_split_opt((Item_func_eq *) cond) &&
        (!retain_ref_cond ||
         !test_if_ref(root_cond, (Item_field*) left_item,right_item)))
    {
      cond->marker=3;
      return (COND*) 0;
    }
  }
  cond->marker=2;
  cond->set_join_tab_idx(join_tab_idx_arg);
  return cond;
}