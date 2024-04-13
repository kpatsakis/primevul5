int cmp_item_datetime::cmp(Item *arg)
{
  const bool rc= value != arg->val_datetime_packed(current_thd);
  return (m_null_value || arg->null_value) ? UNKNOWN : rc;
}