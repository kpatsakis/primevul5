  bool check_is_evaluable_expression_or_error()
  {
    if (is_evaluable_expression())
      return false; // Ok
    raise_error_not_evaluable();
    return true;    // Error
  }