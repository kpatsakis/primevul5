  bool agg_arg_charsets_for_comparison(DTCollation &c,
                                       Item **items, uint nitems,
                                       int item_sep= 1)
  {
    return Type_std_attributes::
      agg_arg_charsets_for_comparison(c, func_name(), items, nitems, item_sep);
  }