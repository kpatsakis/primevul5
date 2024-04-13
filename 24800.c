  bool get_date(THD *thd, MYSQL_TIME *to, date_mode_t mode)
  {
    return decimal_to_datetime_with_warn(thd, VDec(this).ptr(), to, mode,
                                         NULL, NULL);
  }