  longlong val_time_packed_result()
  {
    MYSQL_TIME ltime;
    uint fuzzydate= TIME_TIME_ONLY | TIME_INVALID_DATES | TIME_FUZZY_DATES;
    return get_date_result(&ltime, fuzzydate) ? 0 : pack_time(&ltime);
  }