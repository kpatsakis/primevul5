void st_select_lex::print_order(String *str,
                                ORDER *order,
                                enum_query_type query_type)
{
  for (; order; order= order->next)
  {
    if (order->counter_used)
    {
      char buffer[20];
      size_t length= my_snprintf(buffer, 20, "%d", order->counter);
      str->append(buffer, (uint) length);
    }
    else
    {
      /* replace numeric reference with equivalent for ORDER constant */
      if (order->item[0]->type() == Item::INT_ITEM &&
          order->item[0]->basic_const_item())
      {
        /* make it expression instead of integer constant */
        str->append(STRING_WITH_LEN("''"));
      }
      else
        (*order->item)->print(str, query_type);
    }
    if (order->direction == ORDER::ORDER_DESC)
       str->append(STRING_WITH_LEN(" desc"));
    if (order->next)
      str->append(',');
  }
}