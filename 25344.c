  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    cached_time.copy_to_mysql_time(ltime);
    return (null_value= false);
  }