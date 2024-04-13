Field *Item::create_tmp_field_int(TABLE *table, uint convert_int_length)
{
  const Type_handler *h= &type_handler_long;
  if (max_char_length() > convert_int_length)
    h= &type_handler_longlong;
  return h->make_and_init_table_field(&name, Record_addr(maybe_null),
                                      *this, table);
}