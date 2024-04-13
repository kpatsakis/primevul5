void cmp_item_row::store_value(Item *item)
{
  DBUG_ENTER("cmp_item_row::store_value");
  DBUG_ASSERT(comparators);
  DBUG_ASSERT(n == item->cols());
  item->bring_value();
  item->null_value= 0;
  for (uint i=0; i < n; i++)
  {
    DBUG_ASSERT(comparators[i]);
    comparators[i]->store_value(item->element_index(i));
    item->null_value|= item->element_index(i)->null_value;
  }
  DBUG_VOID_RETURN;
}