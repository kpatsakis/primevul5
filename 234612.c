COND *Item_cond_and::build_equal_items(THD *thd,
                                       COND_EQUAL *inherited,
                                       bool link_item_fields,
                                       COND_EQUAL **cond_equal_ref)
{
  Item_equal *item_equal;
  COND_EQUAL cond_equal;
  cond_equal.upper_levels= inherited;

  if (check_stack_overrun(thd, STACK_MIN_SIZE, NULL))
    return this;                          // Fatal error flag is set!

  List<Item> eq_list;
  List<Item> *cond_args= argument_list();

  List_iterator<Item> li(*cond_args);
  Item *item;

  DBUG_ASSERT(!cond_equal_ref || !cond_equal_ref[0]);
  /*
     Retrieve all conjuncts of this level detecting the equality
     that are subject to substitution by multiple equality items and
     removing each such predicate from the conjunction after having 
     found/created a multiple equality whose inference the predicate is.
 */
  while ((item= li++))
  {
    /*
      PS/SP note: we can safely remove a node from AND-OR
      structure here because it's restored before each
      re-execution of any prepared statement/stored procedure.
    */
    if (item->check_equality(thd, &cond_equal, &eq_list))
      li.remove();
  }

  /*
    Check if we eliminated all the predicates of the level, e.g.
    (a=a AND b=b AND a=a).
  */
  if (!cond_args->elements && 
      !cond_equal.current_level.elements && 
      !eq_list.elements)
    return new (thd->mem_root) Item_int(thd, (longlong) 1, 1);

  List_iterator_fast<Item_equal> it(cond_equal.current_level);
  while ((item_equal= it++))
  {
    item_equal->set_link_equal_fields(link_item_fields);
    item_equal->fix_fields(thd, NULL);
    item_equal->update_used_tables();
    set_if_bigger(thd->lex->current_select->max_equal_elems,
                  item_equal->n_field_items());  
  }

  m_cond_equal.copy(cond_equal);
  cond_equal.current_level= m_cond_equal.current_level;
  inherited= &m_cond_equal;

  /*
     Make replacement of equality predicates for lower levels
     of the condition expression.
  */
  li.rewind();
  while ((item= li++))
  { 
    Item *new_item;
    if ((new_item= item->build_equal_items(thd, inherited, false, NULL))
        != item)
    {
      /* This replacement happens only for standalone equalities */
      /*
        This is ok with PS/SP as the replacement is done for
        cond_args of an AND/OR item, which are restored for each
        execution of PS/SP.
      */
      li.replace(new_item);
    }
  }
  cond_args->append(&eq_list);
  cond_args->append((List<Item> *)&cond_equal.current_level);
  update_used_tables();
  if (cond_equal_ref)
    *cond_equal_ref= &m_cond_equal;
  return this;
}