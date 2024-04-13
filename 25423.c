  const my_decimal *const_ptr_my_decimal() const
  { return can_return_const_value(DECIMAL_RESULT) ? &value.m_decimal : NULL; }