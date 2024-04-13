  bool setup(THD *thd, Item *item)
  {
    if (Item_cache_int::setup(thd, item))
      return true;
    set_if_smaller(decimals, TIME_SECOND_PART_DIGITS);
    return false;
  }