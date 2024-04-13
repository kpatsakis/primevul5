  Item_time_literal(THD *thd, const Time *ltime, uint dec_arg):
    Item_temporal_literal(thd, dec_arg),
    cached_time(*ltime)
  {
    DBUG_ASSERT(cached_time.is_valid_time());
    max_length= MIN_TIME_WIDTH + (decimals ? decimals + 1 : 0);
  }