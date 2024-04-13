  bool get_time(THD *thd, MYSQL_TIME *ltime)
  { return get_date(thd, ltime, Time::Options(thd)); }