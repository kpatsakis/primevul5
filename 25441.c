void Item_func_isnull::print(String *str, enum_query_type query_type)
{
  if (const_item() && !args[0]->maybe_null &&
      !(query_type & (QT_NO_DATA_EXPANSION | QT_VIEW_INTERNAL)))
    str->append("/*always not null*/ 1");
  else
    args[0]->print_parenthesised(str, query_type, precedence());
  str->append(STRING_WITH_LEN(" is null"));
}