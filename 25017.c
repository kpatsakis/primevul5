  longlong val_int()
  {
    return has_value() ? Time(this).to_longlong() : 0;
  }