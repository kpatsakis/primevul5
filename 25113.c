  const MYSQL_TIME *const_ptr_mysql_time() const
  { return can_return_const_value(TIME_RESULT) ? &value.time : NULL; }