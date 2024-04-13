static void unclear_tables(JOIN *join, table_map *cleared_tables)
{
  for (uint i= 0 ; i < join->table_count ; i++)
  {
    if ((*cleared_tables) & (((table_map) 1) << i))
    {
      TABLE *table= join->table[i];
      if (table->s->null_bytes)
        memcpy(table->null_flags, table->record[1], table->s->null_bytes);
      unmark_as_null_row(table);
    }
  }
}