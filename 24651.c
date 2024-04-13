  Item_date_literal(THD *thd, const Date *ltime)
    :Item_temporal_literal(thd),
     cached_time(*ltime)
  {
    DBUG_ASSERT(cached_time.is_valid_date());
    max_length= MAX_DATE_WIDTH;
    /*
      If date has zero month or day, it can return NULL in case of
      NO_ZERO_DATE or NO_ZERO_IN_DATE.
      If date is `February 30`, it can return NULL in case if
      no ALLOW_INVALID_DATES is set.
      We can't set null_value using the current sql_mode here in constructor,
      because sql_mode can change in case of prepared statements
      between PREPARE and EXECUTE.
      Here we only set maybe_null to true if the value has such anomalies.
      Later (during execution time), if maybe_null is true, then the value
      will be checked per row, according to the execution time sql_mode.
      The check_date() below call should cover all cases mentioned.
    */
    maybe_null= cached_time.check_date(TIME_NO_ZERO_DATE | TIME_NO_ZERO_IN_DATE);
  }