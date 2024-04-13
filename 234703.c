Item_cond::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                          uint *and_level, table_map usable_tables,
                          SARGABLE_PARAM **sargables)
{
  List_iterator_fast<Item> li(*argument_list());
  KEY_FIELD *org_key_fields= *key_fields;

  (*and_level)++;
  (li++)->add_key_fields(join, key_fields, and_level, usable_tables,
                         sargables);
  Item *item;
  while ((item=li++))
  {
    KEY_FIELD *start_key_fields= *key_fields;
    (*and_level)++;
    item->add_key_fields(join, key_fields, and_level, usable_tables,
                         sargables);
    *key_fields= merge_key_fields(org_key_fields,start_key_fields,
                                  *key_fields, ++(*and_level));
  }
}