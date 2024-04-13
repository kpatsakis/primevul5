multi_update::~multi_update()
{
  TABLE_LIST *table;
  for (table= update_tables ; table; table= table->next_local)
  {
    table->table->no_keyread= 0;
    if (ignore)
      table->table->file->extra(HA_EXTRA_NO_IGNORE_DUP_KEY);
  }

  if (tmp_tables)
  {
    for (uint cnt = 0; cnt < table_count; cnt++)
    {
      if (tmp_tables[cnt])
      {
	free_tmp_table(thd, tmp_tables[cnt]);
	tmp_table_param[cnt].cleanup();
      }
    }
  }
  if (copy_field)
    delete [] copy_field;
  thd->count_cuted_fields= CHECK_FIELD_IGNORE;		// Restore this setting
  DBUG_ASSERT(trans_safe || !updated || 
              thd->transaction.all.modified_non_trans_table);
}