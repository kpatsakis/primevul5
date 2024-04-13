void Item_in_optimizer::print(String *str, enum_query_type query_type)
{
  if (query_type & QT_PARSABLE)
    args[1]->print(str, query_type);
  else
  {
     restore_first_argument();
     Item_func::print(str, query_type);
  }
}