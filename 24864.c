  bool val_native(THD *thd, Native *to)
  {
    return Time(thd, this).to_native(to, decimals);
  }