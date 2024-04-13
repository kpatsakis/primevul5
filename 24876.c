  Field *create_tmp_field_ex(TABLE *table, Tmp_field_src *src,
                             const Tmp_field_param *param)
  {
    return Item_type_holder::real_type_handler()->
           make_and_init_table_field(&name, Record_addr(maybe_null),
                                     *this, table);
  }