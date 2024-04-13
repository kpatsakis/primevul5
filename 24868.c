  uint cols() const
  { return result_type() == ROW_RESULT ? orig_item->cols() : 1; }