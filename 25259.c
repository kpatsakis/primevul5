int cmp_item_time::cmp(Item *arg)
{
  const bool rc= value != arg->val_time_packed(current_thd);
  return (m_null_value || arg->null_value) ? UNKNOWN : rc;
}