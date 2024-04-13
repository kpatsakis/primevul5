  bool update_null()
  {
    return maybe_null &&
           (null_value= cached_time.check_date_with_warn(current_thd));
  }