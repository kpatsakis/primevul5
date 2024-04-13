  virtual longlong val_datetime_packed()
  {
    MYSQL_TIME ltime;
    uint fuzzydate= TIME_FUZZY_DATES | TIME_INVALID_DATES;
    return get_date_with_conversion(&ltime, fuzzydate) ? 0 : pack_time(&ltime);
  }