  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    if (check_null_ref())
    {
      bzero((char*) ltime,sizeof(*ltime));
      return 1;
    }
    return Item_direct_ref::get_date(thd, ltime, fuzzydate);
  }