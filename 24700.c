void Item_func_like::print(String *str, enum_query_type query_type)
{
  args[0]->print_parenthesised(str, query_type, precedence());
  str->append(' ');
  if (negated)
    str->append(STRING_WITH_LEN(" not "));
  str->append(func_name());
  str->append(' ');
  if (escape_used_in_parsing)
  {
    args[1]->print_parenthesised(str, query_type, precedence());
    str->append(STRING_WITH_LEN(" escape "));
    escape_item->print_parenthesised(str, query_type, higher_precedence());
  }
  else
    args[1]->print_parenthesised(str, query_type, higher_precedence());
}