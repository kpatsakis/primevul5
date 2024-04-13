  Field *create_field_for_schema(THD *thd, TABLE *table)
  { return tmp_table_field_from_field_type(table, false, true); }