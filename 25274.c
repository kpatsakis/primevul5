void Item_func_between::print(String *str, enum_query_type query_type)
{
  args[0]->print_parenthesised(str, query_type, higher_precedence());
  if (negated)
    str->append(STRING_WITH_LEN(" not"));
  str->append(STRING_WITH_LEN(" between "));
  args[1]->print_parenthesised(str, query_type, precedence());
  str->append(STRING_WITH_LEN(" and "));
  args[2]->print_parenthesised(str, query_type, precedence());
}