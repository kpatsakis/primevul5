  longlong val_int()
  {
    return has_value() ? Datetime(this).to_longlong() : 0;
  }