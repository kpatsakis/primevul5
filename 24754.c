Item *Item_in_optimizer::transform(THD *thd, Item_transformer transformer,
                                   uchar *argument)
{
  Item *new_item;

  DBUG_ASSERT(fixed);
  DBUG_ASSERT(!thd->stmt_arena->is_stmt_prepare());
  DBUG_ASSERT(arg_count == 2);

  /* Transform the left IN operand. */
  new_item= (*args)->transform(thd, transformer, argument);
  if (!new_item)
    return 0;
  /*
    THD::change_item_tree() should be called only if the tree was
    really transformed, i.e. when a new item has been created.
    Otherwise we'll be allocating a lot of unnecessary memory for
    change records at each execution.
  */
  if ((*args) != new_item)
    thd->change_item_tree(args, new_item);

  if (invisible_mode())
  {
    /* MAX/MIN transformed => pass through */
    new_item= args[1]->transform(thd, transformer, argument);
    if (!new_item)
      return 0;
    if (args[1] != new_item)
      thd->change_item_tree(args + 1, new_item);
  }
  else
  {
    /*
      Transform the right IN operand which should be an Item_in_subselect or a
      subclass of it. The left operand of the IN must be the same as the left
      operand of this Item_in_optimizer, so in this case there is no further
      transformation, we only make both operands the same.
      TODO: is it the way it should be?
    */
    DBUG_ASSERT((args[1])->type() == Item::SUBSELECT_ITEM &&
                (((Item_subselect*)(args[1]))->substype() ==
                 Item_subselect::IN_SUBS ||
                 ((Item_subselect*)(args[1]))->substype() ==
                 Item_subselect::ALL_SUBS ||
                 ((Item_subselect*)(args[1]))->substype() ==
                 Item_subselect::ANY_SUBS));

    Item_in_subselect *in_arg= (Item_in_subselect*)args[1];
    thd->change_item_tree(&in_arg->left_expr, args[0]);
  }
  return (this->*transformer)(thd, argument);
}