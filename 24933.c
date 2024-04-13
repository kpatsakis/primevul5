  virtual longlong val_datetime_packed(THD *thd)
  {
    return Datetime(thd, this, Datetime::Options_cmp(thd)).to_packed();
  }