Item *in_timestamp::create_item(THD *thd)
{
  return new (thd->mem_root) Item_timestamp_literal(thd);
}