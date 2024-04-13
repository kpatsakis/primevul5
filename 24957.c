void Item_equal::print(String *str, enum_query_type query_type)
{
  if (cond_false)
  {
    str->append('0');
    return;
  }
  str->append(func_name());
  str->append('(');
  List_iterator_fast<Item> it(equal_items);
  Item *item;
  item= it++;
  item->print(str, query_type);
  while ((item= it++))
  {
    str->append(',');
    str->append(' ');
    item->print(str, query_type);
  }
  str->append(')');
}