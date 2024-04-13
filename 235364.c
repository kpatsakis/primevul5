  uint decimal_scale() const
  {
    return decimals < NOT_FIXED_DEC ? decimals :
           is_temporal_type_with_time(field_type()) ?
           TIME_SECOND_PART_DIGITS :
           MY_MIN(max_length, DECIMAL_MAX_SCALE);
  }