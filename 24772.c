void Item_cond_and::mark_as_condition_AND_part(TABLE_LIST *embedding)
{
  List_iterator<Item> li(list);
  Item *item;
  while ((item=li++))
  {
    item->mark_as_condition_AND_part(embedding);
  }
}