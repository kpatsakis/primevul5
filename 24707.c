  const double *const_ptr_double() const
  { return can_return_const_value(REAL_RESULT) ? &value.real : NULL; }