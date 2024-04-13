  uint time_precision(THD *thd)
  {
    return const_item() ? type_handler()->Item_time_precision(thd, this) :
                          MY_MIN(decimals, TIME_SECOND_PART_DIGITS);
  }