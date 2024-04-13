  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    DBUG_ASSERT(0); // never should be called
    return null_value= true;
  }