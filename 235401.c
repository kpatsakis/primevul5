void unfix_fields(List<Item> &fields)
{
  List_iterator<Item> li(fields);
  Item *item;
  while ((item= li++))
    item->unfix_fields();
}