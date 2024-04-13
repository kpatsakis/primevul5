  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  { return get_date_from_int(thd, ltime, fuzzydate); }