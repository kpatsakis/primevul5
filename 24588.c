  Field *create_tmp_field_ex(TABLE *table, Tmp_field_src *src,
                             const Tmp_field_param *param)
  {
    return create_tmp_field_ex_simple(table, src, param);
  }