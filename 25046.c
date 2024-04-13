int cmp_item_timestamp::cmp(Item *arg)
{
  THD *thd= current_thd;
  Timestamp_or_zero_datetime_native_null tmp(thd, arg, true);
  return m_null_value || tmp.is_null() ? UNKNOWN :
         type_handler_timestamp2.cmp_native(m_native, tmp) != 0;
}