void Item_func_in::print(String *str, enum_query_type query_type)
{
  args[0]->print_parenthesised(str, query_type, precedence());
  if (negated)
    str->append(STRING_WITH_LEN(" not"));
  str->append(STRING_WITH_LEN(" in ("));
  print_args(str, 1, query_type);
  str->append(STRING_WITH_LEN(")"));
}