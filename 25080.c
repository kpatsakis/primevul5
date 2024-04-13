static void propagate_and_change_item_tree(THD *thd, Item **place,
                                           COND_EQUAL *cond,
                                           const Item::Context &ctx)
{
  Item *new_value= (*place)->propagate_equal_fields(thd, ctx, cond);
  if (new_value && *place != new_value)
    thd->change_item_tree(place, new_value);
}