void Item_func_not::print(String *str, enum_query_type query_type)
{
  str->append('!');
  args[0]->print_parenthesised(str, query_type, precedence());
}