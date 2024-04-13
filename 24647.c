void Item_func_case_simple::print(String *str, enum_query_type query_type)
{
  Item **pos;
  str->append(STRING_WITH_LEN("case "));
  args[0]->print_parenthesised(str, query_type, precedence());
  str->append(' ');
  print_when_then_arguments(str, query_type, &args[1], when_count());
  if ((pos= Item_func_case_simple::else_expr_addr()))
    print_else_argument(str, query_type, pos[0]);
  str->append(STRING_WITH_LEN("end"));
}