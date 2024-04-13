void Item_func_truth::print(String *str, enum_query_type query_type)
{
  args[0]->print_parenthesised(str, query_type, precedence());
  str->append(STRING_WITH_LEN(" is "));
  if (! affirmative)
    str->append(STRING_WITH_LEN("not "));
  if (value)
    str->append(STRING_WITH_LEN("true"));
  else
    str->append(STRING_WITH_LEN("false"));
}