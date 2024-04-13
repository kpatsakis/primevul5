void multi_update::prepare_to_read_rows()
{
  /*
    update column maps now. it cannot be done in ::prepare() before the
    optimizer, because the optimize might reset them (in
    SELECT_LEX::update_used_tables()), it cannot be done in
    ::initialize_tables() after the optimizer, because the optimizer
    might read rows from const tables
  */

  for (TABLE_LIST *tl= update_tables; tl; tl= tl->next_local)
    tl->table->mark_columns_needed_for_update();
}