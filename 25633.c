Field *Item::create_field_for_schema(THD *thd, TABLE *table)
{
  if (field_type() == MYSQL_TYPE_VARCHAR)
  {
    Field *field;
    if (max_length > MAX_FIELD_VARCHARLENGTH)
      field= new Field_blob(max_length, maybe_null, name, collation.collation);
    else
      field= new Field_varstring(max_length, maybe_null, name,
                                 table->s, collation.collation);
    if (field)
      field->init(table);
    return field;
  }
  return tmp_table_field_from_field_type(table, false, false);
}