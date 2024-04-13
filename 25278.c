  Item_temporal_literal(THD *thd, uint dec_arg):
    Item_literal(thd)
  {
    collation.set(&my_charset_numeric, DERIVATION_NUMERIC, MY_REPERTOIRE_ASCII);
    decimals= dec_arg;
  }