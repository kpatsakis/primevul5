void Item_is_not_null_test::update_used_tables()
{
  if (!args[0]->maybe_null)
    used_tables_cache= 0;			/* is always true */
  else
    args[0]->update_used_tables();
}