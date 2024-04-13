  bool get_date(MYSQL_TIME *ltime, ulonglong fuzzy_date)
  {
    *ltime= cached_time;
    return (null_value= false);
  }