  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    return type_handler()->Item_get_date_with_warn(thd, this, ltime, fuzzydate);
  }