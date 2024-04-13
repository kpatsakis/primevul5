void Item_cond::print(String *str, enum_query_type query_type)
{
  List_iterator_fast<Item> li(list);
  Item *item;
  if ((item=li++))
    item->print_parenthesised(str, query_type, precedence());
  while ((item=li++))
  {
    str->append(' ');
    str->append(func_name());
    str->append(' ');
    item->print_parenthesised(str, query_type, precedence());
  }
}