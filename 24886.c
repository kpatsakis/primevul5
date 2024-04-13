void in_row::set(uint pos, Item *item)
{
  DBUG_ENTER("in_row::set");
  DBUG_PRINT("enter", ("pos: %u  item: %p", pos,item));
  ((cmp_item_row*) base)[pos].store_value_by_template(current_thd, &tmp, item);
  DBUG_VOID_RETURN;
}