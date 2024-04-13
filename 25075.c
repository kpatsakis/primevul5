  Item_datetime_literal(THD *thd, const Datetime *ltime, uint dec_arg):
    Item_temporal_literal(thd, dec_arg),
    cached_time(*ltime)
  {
    DBUG_ASSERT(cached_time.is_valid_datetime());
    max_length= MAX_DATETIME_WIDTH + (decimals ? decimals + 1 : 0);
    // See the comment on maybe_null in Item_date_literal
    maybe_null= cached_time.check_date(TIME_NO_ZERO_DATE | TIME_NO_ZERO_IN_DATE);
  }