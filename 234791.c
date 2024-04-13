bool JOIN::inject_cond_into_where(Item *injected_cond)
{
  Item *where_item= injected_cond;
  List<Item> *and_args= NULL;
  if (conds && conds->type() == Item::COND_ITEM &&
      ((Item_cond*) conds)->functype() == Item_func::COND_AND_FUNC)
  {
    and_args= ((Item_cond*) conds)->argument_list();
    if (cond_equal)
      and_args->disjoin((List<Item> *) &cond_equal->current_level);
  }

  where_item= and_items(thd, conds, where_item);
  if (where_item->fix_fields_if_needed(thd, 0))
    return true;
  thd->change_item_tree(&select_lex->where, where_item);
  select_lex->where->top_level_item();
  conds= select_lex->where;

  if (and_args && cond_equal)
  {
    and_args= ((Item_cond*) conds)->argument_list();
    List_iterator<Item_equal> li(cond_equal->current_level);
    Item_equal *elem;
    while ((elem= li++))
    {
      and_args->push_back(elem, thd->mem_root);
    }
  }

  return false;

}