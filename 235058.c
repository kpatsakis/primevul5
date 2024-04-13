  Item_time_literal(THD *thd, MYSQL_TIME *ltime, uint dec_arg):
    Item_temporal_literal(thd, ltime, dec_arg)
  {
    max_length= MIN_TIME_WIDTH + (decimals ? decimals + 1 : 0);
    fixed= 1;
  }