Item *in_string::create_item(THD *thd)
{
  return new (thd->mem_root) Item_string_for_in_vector(thd, collation);
}