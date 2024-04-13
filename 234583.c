Item_cond_and::add_key_fields(JOIN *join, KEY_FIELD **key_fields,
                              uint *and_level, table_map usable_tables,
                              SARGABLE_PARAM **sargables)
{
  List_iterator_fast<Item> li(*argument_list());
  KEY_FIELD *org_key_fields= *key_fields;

  Item *item;
  while ((item=li++))
    item->add_key_fields(join, key_fields, and_level, usable_tables,
                         sargables);
  for (; org_key_fields != *key_fields ; org_key_fields++)
    org_key_fields->level= *and_level;
}