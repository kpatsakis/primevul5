bool generate_derived_keys_for_table(KEYUSE *keyuse, uint count, uint keys)
{
  TABLE *table= keyuse->table;
  if (table->alloc_keys(keys))
    return TRUE;
  uint key_count= 0;
  KEYUSE *first_keyuse= keyuse;
  uint prev_part= keyuse->keypart;
  uint parts= 0;
  uint i= 0;

  for ( ; i < count && key_count < keys; )
  {
    do
    {
      keyuse->key= table->s->keys;
      keyuse->keypart_map= (key_part_map) (1 << parts);     
      keyuse++;
      i++;
    } 
    while (i < count && keyuse->used_tables == first_keyuse->used_tables &&
           keyuse->keypart == prev_part);
    parts++;
    if (i < count && keyuse->used_tables == first_keyuse->used_tables)
    {
      prev_part= keyuse->keypart;
    }
    else
    {
      KEYUSE *save_first_keyuse= first_keyuse;
      if (table->check_tmp_key(table->s->keys, parts,
                               get_next_field_for_derived_key_simple,
                               (uchar *) &first_keyuse))

      {
        first_keyuse= save_first_keyuse;
        if (table->add_tmp_key(table->s->keys, parts, 
                               get_next_field_for_derived_key, 
                               (uchar *) &first_keyuse,
                               FALSE))
          return TRUE;
        table->reginfo.join_tab->keys.set_bit(table->s->keys);
      }
      else
      {
        /* Mark keyuses for this key to be excluded */
        for (KEYUSE *curr=save_first_keyuse; curr < keyuse; curr++)
	{
          curr->key= MAX_KEY;
        }
      }
      first_keyuse= keyuse;
      key_count++;
      parts= 0;
      prev_part= keyuse->keypart;
    }
  }             

  return FALSE;
}