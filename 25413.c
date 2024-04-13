  void quick_fix_field()
  {
    /*
      We can get here when Item_bool is created instead of a constant
      predicate at various condition optimization stages in sql_select.
    */
  }