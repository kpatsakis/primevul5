  virtual Field *create_tmp_field(bool group, TABLE *table)
  {
    /*
      Values with MY_INT32_NUM_DECIMAL_DIGITS digits may or may not fit into
      Field_long : make them Field_longlong.
    */
    return create_tmp_field(false, table, MY_INT32_NUM_DECIMAL_DIGITS - 2);
  }