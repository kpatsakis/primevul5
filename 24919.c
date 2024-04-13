  const longlong *const_ptr_longlong() const
  { return can_return_const_value(INT_RESULT) ? &value.integer : NULL; }