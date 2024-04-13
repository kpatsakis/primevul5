  bool excl_dep_on_table(table_map tab_map)
  { 
    table_map used= used_tables();
    if (used & OUTER_REF_TABLE_BIT)
      return false;
    return (used == tab_map) || (*ref)->excl_dep_on_table(tab_map);
  }