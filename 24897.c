  bool val_native(THD *thd, Native *to)
  {
    DBUG_ASSERT(sane());
    return null_value || m_value.to_native(to, decimals);
  }