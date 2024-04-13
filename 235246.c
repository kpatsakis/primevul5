  bool convert_time_to_datetime(THD *thd, MYSQL_TIME *ltime, ulonglong fuzzydate)
  {
    MYSQL_TIME tmp;
    if (time_to_datetime_with_warn(thd, ltime, &tmp, fuzzydate))
      return null_value= true;
    *ltime= tmp;
    return false;
  }