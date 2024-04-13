  double val_real()
  {
    return has_value() ? Time(this).to_double() : 0;
  }