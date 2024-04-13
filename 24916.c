  void update_table_bitmaps()
  {
    if (field && field->table)
    {
      TABLE *tab= field->table;
      tab->covering_keys.intersect(field->part_of_key);
      if (tab->read_set)
        tab->mark_column_with_deps(field);
    }
  }