  bool get_date(MYSQL_TIME *ltime, ulonglong fuzzydate)
  {
    if (check_null_ref())
    {
      bzero((char*) ltime,sizeof(*ltime));
      return 1;
    }
    return Item_direct_ref::get_date(ltime, fuzzydate);
  }