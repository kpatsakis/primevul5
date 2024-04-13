  uint divisor_precision_increment() const
  {
    return decimals <  NOT_FIXED_DEC ? decimals :
           is_temporal_type_with_time(field_type()) ?
           TIME_SECOND_PART_DIGITS :
           decimals;
  }