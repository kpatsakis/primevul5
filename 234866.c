  Item_temporal_literal(THD *thd, MYSQL_TIME *ltime): Item_basic_constant(thd)
  {
    collation.set(&my_charset_numeric, DERIVATION_NUMERIC, MY_REPERTOIRE_ASCII);
    decimals= 0;
    cached_time= *ltime;
  }