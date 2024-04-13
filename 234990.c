  virtual longlong val_time_packed()
  {
    MYSQL_TIME ltime;
    uint fuzzydate= TIME_FUZZY_DATES | TIME_INVALID_DATES | TIME_TIME_ONLY;
    return get_date(&ltime, fuzzydate) ? 0 : pack_time(&ltime);
  }