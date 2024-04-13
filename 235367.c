  inline uint float_length(uint decimals_par) const
  { return decimals < FLOATING_POINT_DECIMALS ? (DBL_DIG+2+decimals_par) : DBL_DIG+8;}