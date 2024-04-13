  bool get_date_from_item(THD *thd, Item *item,
                          MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    bool rc= item->get_date(thd, ltime, fuzzydate);
    null_value= MY_TEST(rc || item->null_value);
    return rc;
  }