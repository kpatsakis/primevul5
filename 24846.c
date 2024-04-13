Item *Item_cond::propagate_equal_fields(THD *thd,
                                        const Context &ctx,
                                        COND_EQUAL *cond)
{
  DBUG_ASSERT(!thd->stmt_arena->is_stmt_prepare());
  DBUG_ASSERT(arg_count == 0);
  List_iterator<Item> li(list);
  while (li++)
  {
    /*
      The exact value of the last parameter to propagate_and_change_item_tree()
      is not important at this point. Item_func derivants will create and
      pass their own context to the arguments.
    */
    propagate_and_change_item_tree(thd, li.ref(), cond, Context_boolean());
  }
  return this;
}