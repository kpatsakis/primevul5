  Field *create_table_field_from_handler(TABLE *table)
  {
    const Type_handler *h= type_handler();
    return h->make_and_init_table_field(&name, Record_addr(maybe_null),
                                        *this, table);
  }