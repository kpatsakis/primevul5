  Field *tmp_table_field_from_field_type(TABLE *table)
  {
    DBUG_ASSERT(is_fixed());
    const Type_handler *h= type_handler()->type_handler_for_tmp_table(this);
    return h->make_and_init_table_field(&name, Record_addr(maybe_null),
                                        *this, table);
  }