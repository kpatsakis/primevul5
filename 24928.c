int cmp_item_time::cmp_not_null(const Value *val)
{
  DBUG_ASSERT(!val->is_null());
  DBUG_ASSERT(val->is_temporal());
  return value != pack_time(&val->value.m_time);
}