bool generate_derived_keys(DYNAMIC_ARRAY *keyuse_array)
{
  KEYUSE *keyuse= dynamic_element(keyuse_array, 0, KEYUSE*);
  uint elements= keyuse_array->elements;
  TABLE *prev_table= 0;
  for (uint i= 0; i < elements; i++, keyuse++)
  {
    if (!keyuse->table)
      break;
    KEYUSE *first_table_keyuse= NULL;
    table_map last_used_tables= 0;
    uint count= 0;
    uint keys= 0;
    TABLE_LIST *derived= NULL;
    if (keyuse->table != prev_table)
      derived= keyuse->table->pos_in_table_list;
    while (derived && derived->is_materialized_derived())
    {
      if (keyuse->table != prev_table)
      {
        prev_table= keyuse->table;
        while (keyuse->table == prev_table && keyuse->key != MAX_KEY)
	{
          keyuse++;
          i++;
        }
        if (keyuse->table != prev_table)
	{
          keyuse--;
          i--;
          derived= NULL;
          continue;
        }
        first_table_keyuse= keyuse;
        last_used_tables= keyuse->used_tables;
        count= 0;
        keys= 0;
      }
      else if (keyuse->used_tables != last_used_tables)
      {
        keys++;
        last_used_tables= keyuse->used_tables;
      }
      count++;
      keyuse++;
      i++;
      if (keyuse->table != prev_table)
      {
        if (generate_derived_keys_for_table(first_table_keyuse, count, ++keys))
          return TRUE;
        keyuse--;
        i--;
	derived= NULL;
      }
    }
  }
  return FALSE;
}