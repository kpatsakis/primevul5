static void clear_tables(JOIN *join, table_map *cleared_tables)
{
  /* 
    must clear only the non-const tables as const tables are not re-calculated.
  */
  for (uint i= 0 ; i < join->table_count ; i++)
  {
    TABLE *table= join->table[i];

    if (table->null_row)
      continue;                                 // Nothing more to do
    if (!(table->map & join->const_table_map) || cleared_tables)
    {
      if (cleared_tables)
      {
        (*cleared_tables)|= (((table_map) 1) << i);
        if (table->s->null_bytes)
        {
          /*
            Remember null bits for the record so that we can restore the
            original const record in unclear_tables()
          */
          memcpy(table->record[1], table->null_flags, table->s->null_bytes);
        }
      }
      mark_as_null_row(table);                  // All fields are NULL
    }
  }
}