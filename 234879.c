  bool agg_arg_charsets_for_comparison(CHARSET_INFO **cs, Item **a, Item **b)
  {
    DTCollation tmp;
    if (tmp.set((*a)->collation, (*b)->collation, MY_COLL_CMP_CONV) ||
        tmp.derivation == DERIVATION_NONE)
    {
      my_error(ER_CANT_AGGREGATE_2COLLATIONS,MYF(0),
               (*a)->collation.collation->name,
               (*a)->collation.derivation_name(),
               (*b)->collation.collation->name,
               (*b)->collation.derivation_name(),
               func_name());
      return true;
    }
    if (agg_item_set_converter(tmp, func_name(),
                               a, 1, MY_COLL_CMP_CONV, 1) ||
        agg_item_set_converter(tmp, func_name(),
                               b, 1, MY_COLL_CMP_CONV, 1))
      return true;
    *cs= tmp.collation;
    return false;
  }