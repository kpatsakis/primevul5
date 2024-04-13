  const String *const_ptr_string() const
  { return can_return_const_value(STRING_RESULT) ? &value.m_string : NULL; }