int cmp_item_timestamp::compare(cmp_item *arg)
{
  cmp_item_timestamp *tmp= static_cast<cmp_item_timestamp*>(arg);
  return type_handler_timestamp2.cmp_native(m_native, tmp->m_native);
}