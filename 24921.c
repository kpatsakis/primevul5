  Field *create_tmp_field_ex(TABLE *table, Tmp_field_src *src,
                             const Tmp_field_param *param)
  {
    /*
      We can get to here when using a CURSOR for a query with NAME_CONST():
        DECLARE c CURSOR FOR SELECT NAME_CONST('x','y') FROM t1;
        OPEN c;
    */
    return tmp_table_field_from_field_type_maybe_null(table, src, param,
                                              type() == Item::NULL_ITEM);
  }