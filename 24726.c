int cmp_item_decimal::cmp_not_null(const Value *val)
{
  DBUG_ASSERT(!val->is_null());
  DBUG_ASSERT(val->is_decimal());
  return my_decimal_cmp(&value, &val->m_decimal);
}