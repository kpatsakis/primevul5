TABLE *find_locked_table(TABLE *list, const char *db, const char *table_name)
{
  char	key[MAX_DBKEY_LENGTH];
  uint key_length= tdc_create_key(key, db, table_name);

  for (TABLE *table= list; table ; table=table->next)
  {
    if (table->s->table_cache_key.length == key_length &&
	!memcmp(table->s->table_cache_key.str, key, key_length))
      return table;
  }
  return(0);
}