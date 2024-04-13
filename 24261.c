static void mark_real_tables_as_free_for_reuse(TABLE_LIST *table_list)
{
  TABLE_LIST *table;
  for (table= table_list; table; table= table->next_global)
    if (!table->placeholder())
    {
      table->table->query_id= 0;
    }
  for (table= table_list; table; table= table->next_global)
    if (!table->placeholder())
    {
      /*
        Detach children of MyISAMMRG tables used in
        sub-statements, they will be reattached at open.
        This has to be done in a separate loop to make sure
        that children have had their query_id cleared.
      */
      table->table->file->extra(HA_EXTRA_DETACH_CHILDREN);
    }
}