  bool const_item() const
  {
    return (*ref)->const_item() && (null_ref_table == NO_NULL_TABLE);
  }