  Item_date_literal(THD *thd, MYSQL_TIME *ltime)
    :Item_temporal_literal(thd, ltime)
  {
    max_length= MAX_DATE_WIDTH;
    fixed= 1;
    /*
      If date has zero month or day, it can return NULL in case of
      NO_ZERO_DATE or NO_ZERO_IN_DATE.
      We can't just check the current sql_mode here in constructor,
      because sql_mode can change in case of prepared statements
      between PREPARE and EXECUTE.
    */
    maybe_null= !ltime->month || !ltime->day;
  }