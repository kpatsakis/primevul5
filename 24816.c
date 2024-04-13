  double val_real()
  {
    return has_value() ? Datetime(this).to_double() : 0;
  }