void Item_func_case_searched::print(String *str, enum_query_type query_type)
{
  Item **pos;
  str->append(STRING_WITH_LEN("case "));
  print_when_then_arguments(str, query_type, &args[0], when_count());
  if ((pos= Item_func_case_searched::else_expr_addr()))
    print_else_argument(str, query_type, pos[0]);
  str->append(STRING_WITH_LEN("end"));
}