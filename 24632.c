  uint cols() const
  {
    return ref && result_type() == ROW_RESULT ? (*ref)->cols() : 1;
  }