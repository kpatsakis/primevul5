  table_map not_null_tables() const 
  { 
    return depended_from ? 0 : (*ref)->not_null_tables();
  }