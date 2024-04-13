void Item_func_decode_oracle::print(String *str, enum_query_type query_type)
{
  str->append(func_name());
  str->append('(');
  args[0]->print(str, query_type);
  for (uint i= 1, count= when_count() ; i <= count; i++)
  {
    str->append(',');
    args[i]->print(str, query_type);
    str->append(',');
    args[i+count]->print(str, query_type);
  }
  Item **else_expr= Item_func_case_simple::else_expr_addr();
  if (else_expr)
  {
    str->append(',');
    (*else_expr)->print(str, query_type);
  }
  str->append(')');
}