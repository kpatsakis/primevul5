  double val_real()
  {
    return m_value.to_datetime(current_thd).to_double();
  }