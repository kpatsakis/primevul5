  bool agg_arg_charsets(DTCollation &c, Item **items, uint nitems,
                        uint flags, int item_sep)
  {
    return Type_std_attributes::agg_arg_charsets(c, func_name(),
                                                 items, nitems,
                                                 flags, item_sep);
  }