  bool get_date(THD *thd, MYSQL_TIME *to, date_mode_t mode)
  {
    return type_handler_year.Item_get_date_with_warn(thd, this, to, mode);
  }