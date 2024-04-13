  Item_datetime_literal_for_invalid_dates(THD *thd,
                                          const Datetime *ltime, uint dec_arg)
   :Item_datetime_literal(thd, ltime, dec_arg)
  {
    maybe_null= false;
  }