  Double_null to_double_null()
  {
    // val_real() must be caleed on a separate line. See to_longlong_null()
    double nr= val_real();
    return Double_null(nr, null_value);
  }