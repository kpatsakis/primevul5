void Item_func_isnotnull::print(String *str, enum_query_type query_type)
{
  args[0]->print_parenthesised(str, query_type, precedence());
  str->append(STRING_WITH_LEN(" is not null"));
}