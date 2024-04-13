  bool too_big_for_varchar() const
  { return max_char_length() > CONVERT_IF_BIGGER_TO_BLOB; }