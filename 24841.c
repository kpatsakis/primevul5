  bool val_native(THD *thd, Native *to)
  {
    return has_value() ? Time(thd, this).to_native(to, decimals) : true;
  }