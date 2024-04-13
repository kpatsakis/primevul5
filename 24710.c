  Field *create_tmp_field_ex_simple(TABLE *table,
                                    Tmp_field_src *src,
                                    const Tmp_field_param *param)
  {
    DBUG_ASSERT(!param->make_copy_field());
    DBUG_ASSERT(!is_result_field());
    DBUG_ASSERT(type() != NULL_ITEM);
    return tmp_table_field_from_field_type(table);
  }