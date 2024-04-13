static void reset_thd_trn(THD *thd, MARIA_HA *first_table)
{
  DBUG_ENTER("reset_thd_trn");
  thd_set_ha_data(thd, maria_hton, 0);
  MARIA_HA *next;
  for (MARIA_HA *table= first_table; table ; table= next)
  {
    next= table->trn_next;
    _ma_reset_trn_for_table(table);

    /*
      If table has changed by this statement, invalidate it from the query
      cache
    */
    if (table->row_changes != table->start_row_changes)
    {
      table->start_row_changes= table->row_changes;
      DBUG_ASSERT(table->s->chst_invalidator != NULL);
      (*table->s->chst_invalidator)(table->s->data_file_name.str);
    }
  }
  DBUG_VOID_RETURN;
}