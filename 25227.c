int cmp_item_decimal::cmp(Item *arg)
{
  VDec tmp(arg);
  return m_null_value || tmp.is_null() ? UNKNOWN : (tmp.cmp(&value) != 0);
}