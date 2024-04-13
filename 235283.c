  Item** addr(uint i)
  { return result_type() == ROW_RESULT ? orig_item->addr(i) : 0; }