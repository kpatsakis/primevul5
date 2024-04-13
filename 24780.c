inline void Item_func_case::print_else_argument(String *str,
                                                enum_query_type query_type,
                                                Item *item)
{
  str->append(STRING_WITH_LEN("else "));
  item->print(str, query_type);
  str->append(' ');
}