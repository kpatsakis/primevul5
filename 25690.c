static void optimize_keyuse(JOIN *join, DYNAMIC_ARRAY *keyuse_array)
{
  KEYUSE *end,*keyuse= dynamic_element(keyuse_array, 0, KEYUSE*);

  for (end= keyuse+ keyuse_array->elements ; keyuse < end ; keyuse++)
  {
    table_map map;
    /*
      If we find a ref, assume this table matches a proportional
      part of this table.
      For example 100 records matching a table with 5000 records
      gives 5000/100 = 50 records per key
      Constant tables are ignored.
      To avoid bad matches, we don't make ref_table_rows less than 100.
    */
    keyuse->ref_table_rows= ~(ha_rows) 0;	// If no ref
    if (keyuse->used_tables &
	(map= (keyuse->used_tables & ~join->const_table_map &
	       ~OUTER_REF_TABLE_BIT)))
    {
      uint n_tables= my_count_bits(map);
      if (n_tables == 1)			// Only one table
      {
        DBUG_ASSERT(!(map & PSEUDO_TABLE_BITS)); // Must be a real table
        Table_map_iterator it(map);
        int tablenr= it.next_bit();
        DBUG_ASSERT(tablenr != Table_map_iterator::BITMAP_END);
	TABLE *tmp_table=join->table[tablenr];
        if (tmp_table) // already created
          keyuse->ref_table_rows= MY_MAX(tmp_table->file->stats.records, 100);
      }
    }
    /*
      Outer reference (external field) is constant for single executing
      of subquery
    */
    if (keyuse->used_tables == OUTER_REF_TABLE_BIT)
      keyuse->ref_table_rows= 1;
  }
}