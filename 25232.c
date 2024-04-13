  longlong val_datetime_packed(THD *thd)
  {
    Datetime::Options_cmp opt(thd);
    return has_value() ? Datetime(thd, this, opt).to_packed() : 0;
  }