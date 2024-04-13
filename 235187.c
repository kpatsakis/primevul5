  bool get_date_from_item(Item *item, MYSQL_TIME *ltime, ulonglong fuzzydate)
  {
    bool rc= item->get_date(ltime, fuzzydate);
    null_value= MY_TEST(rc || item->null_value);
    return rc;
  }