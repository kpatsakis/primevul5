  Item_date_literal_for_invalid_dates(THD *thd, const Date *ltime)
   :Item_date_literal(thd, ltime)
  {
    maybe_null= false;
  }