  double val_real()
  {
    DBUG_ASSERT(sane());
    return null_value ? 0e0 :
           m_value.to_datetime(current_thd).to_double();
  }