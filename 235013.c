  bool null_inside()
  { return result_type() == ROW_RESULT ? orig_item->null_inside() : 0; }