  bool get_date(THD *thd, MYSQL_TIME *to, date_mode_t fuzzydate)
  {
    *to= ltime;
    return false;
  }