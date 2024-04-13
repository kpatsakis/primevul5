  virtual table_map used_tables() const
  {
    return state != NO_VALUE ? (table_map)0 : PARAM_TABLE_BIT;
  }