  bool val_native(THD *thd, Native *to)
  {
    return m_value.to_native(to, decimals);
  }