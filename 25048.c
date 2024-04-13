  virtual uint decimal_precision() const
  {
    return type_handler()->Item_decimal_precision(this);
  }