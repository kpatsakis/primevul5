  void update_table_bitmaps()
  {
    if (field && field->table)
    {
      TABLE *tab= field->table;
      tab->covering_keys.intersect(field->part_of_key);
      if (tab->read_set)
        bitmap_fast_test_and_set(tab->read_set, field->field_index);
      /* 
        Do not mark a self-referecing virtual column.
        Such virtual columns are reported as invalid.
      */
      if (field->vcol_info && tab->vcol_set)
        tab->mark_virtual_col(field);
    }
  }