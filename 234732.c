join_read_system(JOIN_TAB *tab)
{
  TABLE *table= tab->table;
  int error;
  if (table->status & STATUS_GARBAGE)		// If first read
  {
    if (unlikely((error=
                  table->file->ha_read_first_row(table->record[0],
                                                 table->s->primary_key))))
    {
      if (error != HA_ERR_END_OF_FILE)
	return report_error(table, error);
      table->const_table= 1;
      mark_as_null_row(tab->table);
      empty_record(table);			// Make empty record
      return -1;
    }
    store_record(table,record[1]);
  }
  else if (!table->status)			// Only happens with left join
    restore_record(table,record[1]);			// restore old record
  table->null_row=0;
  return table->status ? -1 : 0;
}