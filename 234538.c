void JOIN::reset_query_plan()
{
  for (uint i= 0; i < table_count; i++)
  {
    join_tab[i].keyuse= NULL;
    join_tab[i].checked_keys.clear_all();
  }
}