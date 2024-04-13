  bool agg_arg_charsets(DTCollation &c, Item **items, uint nitems,
                        uint flags, int item_sep)
  {
    if (agg_item_collations(c, func_name(), items, nitems, flags, item_sep))
      return true;

    return agg_item_set_converter(c, func_name(), items, nitems,
                                  flags, item_sep);
  }