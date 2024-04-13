  Item* element_index(uint i)
  { return result_type() == ROW_RESULT ? orig_item->element_index(i) : this; }