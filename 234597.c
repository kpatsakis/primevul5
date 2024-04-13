Item_equal::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                           uint *and_level, table_map usable_tables,
                           SARGABLE_PARAM **sargables)
{
  Item *const_item2= get_const();
  Item_equal_fields_iterator it(*this);
  if (const_item2)
  {
    
    /*
      For each field field1 from item_equal consider the equality 
      field1=const_item as a condition allowing an index access of the table
      with field1 by the keys value of field1.
    */   
    while (it++)
    {
      Field *equal_field= it.get_curr_field();
      add_key_field(join, key_fields, *and_level, this, equal_field,
                    TRUE, &const_item2, 1, usable_tables, sargables);
    }
  }
  else 
  {
    /*
      Consider all pairs of different fields included into item_equal.
      For each of them (field1, field1) consider the equality 
      field1=field2 as a condition allowing an index access of the table
      with field1 by the keys value of field2.
    */   
    Item_equal_fields_iterator fi(*this);
    while (fi++)
    {
      Field *field= fi.get_curr_field();
      Item *item;
      while ((item= it++))
      {
        Field *equal_field= it.get_curr_field();
        if (!field->eq(equal_field))
        {
          add_key_field(join, key_fields, *and_level, this, field,
                        TRUE, &item, 1, usable_tables,
                        sargables);
        }
      }
      it.rewind();
    }
  }
}