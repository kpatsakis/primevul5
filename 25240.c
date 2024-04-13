  virtual bool get_date_result(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  { return get_date(thd, ltime,fuzzydate); }