inline void Item_func_case::print_when_then_arguments(String *str,
                                                      enum_query_type
                                                      query_type,
                                                      Item **items, uint count)
{
  for (uint i= 0; i < count; i++)
  {
    str->append(STRING_WITH_LEN("when "));
    items[i]->print(str, query_type);
    str->append(STRING_WITH_LEN(" then "));
    items[i + count]->print(str, query_type);
    str->append(' ');
  }
}