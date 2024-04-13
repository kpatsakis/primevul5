Item_func_between::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                                  uint *and_level, table_map usable_tables,
                                  SARGABLE_PARAM **sargables)
{
  /*
    Build list of possible keys for 'a BETWEEN low AND high'.
    It is handled similar to the equivalent condition 
    'a >= low AND a <= high':
  */
  Item_field *field_item;
  bool equal_func= false;
  uint num_values= 2;

  bool binary_cmp= (args[0]->real_item()->type() == Item::FIELD_ITEM)
        ? ((Item_field*) args[0]->real_item())->field->binary()
        : true;
  /*
    Additional optimization: If 'low = high':
    Handle as if the condition was "t.key = low".
  */
  if (!negated && args[1]->eq(args[2], binary_cmp))
  {
    equal_func= true;
    num_values= 1;
  }

  /*
    Append keys for 'field <cmp> value[]' if the
    condition is of the form::
    '<field> BETWEEN value[1] AND value[2]'
  */
  if (is_local_field(args[0]))
  {
    field_item= (Item_field *) (args[0]->real_item());
    add_key_equal_fields(join, key_fields, *and_level, this,
                         field_item, equal_func, &args[1],
                         num_values, usable_tables, sargables);
  }
  /*
    Append keys for 'value[0] <cmp> field' if the
    condition is of the form:
    'value[0] BETWEEN field1 AND field2'
  */
  for (uint i= 1; i <= num_values; i++)
  {
    if (is_local_field(args[i]))
    {
      field_item= (Item_field *) (args[i]->real_item());
      add_key_equal_fields(join, key_fields, *and_level, this,
                           field_item, equal_func, args,
                           1, usable_tables, sargables);
    }
  }
}