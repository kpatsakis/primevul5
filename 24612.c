  double val_real()
  {
    return update_null() ? 0 : cached_time.to_double();
  }