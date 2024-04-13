  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
  {
    DBUG_ASSERT(sane());
    bool res= m_value.to_TIME(thd, ltime, fuzzydate);
    DBUG_ASSERT(!res);
    return null_value || res;
  }