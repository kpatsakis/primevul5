int append_possible_keys(MEM_ROOT *alloc, String_list &list, TABLE *table, 
                         key_map possible_keys)
{
  uint j;
  for (j=0 ; j < table->s->keys ; j++)
  {
    if (possible_keys.is_set(j))
      list.append_str(alloc, table->key_info[j].name);
  }
  return 0;
}