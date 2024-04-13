  bool get_time(MYSQL_TIME *ltime)
  { return get_date(ltime, TIME_TIME_ONLY | TIME_INVALID_DATES); }