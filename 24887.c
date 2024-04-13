int cmp_item_timestamp::cmp_not_null(const Value *val)
{
  /*
    This method will be implemented when we add this syntax:
      SELECT TIMESTAMP WITH LOCAL TIME ZONE '2001-01-01 10:20:30'
    For now TIMESTAMP is compared to non-TIMESTAMP using DATETIME.
  */
  DBUG_ASSERT(0);
  return 0;
}