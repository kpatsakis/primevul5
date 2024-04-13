bool unsafe_key_update(List<TABLE_LIST> leaves, table_map tables_for_update)
{
  List_iterator_fast<TABLE_LIST> it(leaves), it2(leaves);
  TABLE_LIST *tl, *tl2;

  while ((tl= it++))
  {
    if (!tl->is_jtbm() && (tl->table->map & tables_for_update))
    {
      TABLE *table1= tl->table;
      bool primkey_clustered= (table1->file->primary_key_is_clustered() &&
                               table1->s->primary_key != MAX_KEY);

      bool table_partitioned= false;
#ifdef WITH_PARTITION_STORAGE_ENGINE
      table_partitioned= (table1->part_info != NULL);
#endif

      if (!table_partitioned && !primkey_clustered)
        continue;

      it2.rewind();
      while ((tl2= it2++))
      {
        if (tl2->is_jtbm())
          continue;
        /*
          Look at "next" tables only since all previous tables have
          already been checked
        */
        TABLE *table2= tl2->table;
        if (tl2 != tl &&
            table2->map & tables_for_update && table1->s == table2->s)
        {
          // A table is updated through two aliases
          if (table_partitioned &&
              (partition_key_modified(table1, table1->write_set) ||
               partition_key_modified(table2, table2->write_set)))
          {
            // Partitioned key is updated
            my_error(ER_MULTI_UPDATE_KEY_CONFLICT, MYF(0),
                     tl->top_table()->alias.str,
                     tl2->top_table()->alias.str);
            return true;
          }

          if (primkey_clustered)
          {
            // The primary key can cover multiple columns
            KEY key_info= table1->key_info[table1->s->primary_key];
            KEY_PART_INFO *key_part= key_info.key_part;
            KEY_PART_INFO *key_part_end= key_part + key_info.user_defined_key_parts;

            for (;key_part != key_part_end; ++key_part)
            {
              if (bitmap_is_set(table1->write_set, key_part->fieldnr-1) ||
                  bitmap_is_set(table2->write_set, key_part->fieldnr-1))
              {
                // Clustered primary key is updated
                my_error(ER_MULTI_UPDATE_KEY_CONFLICT, MYF(0),
                         tl->top_table()->alias.str,
                         tl2->top_table()->alias.str);
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}