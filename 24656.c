  longlong val_time_packed(THD *thd)
  {
    return Time(thd, this, Time::Options_cmp(thd)).to_packed();
  }