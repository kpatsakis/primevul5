join_read_last(JOIN_TAB *tab)
{
  TABLE *table=tab->table;
  int error= 0;
  DBUG_ENTER("join_read_last");

  DBUG_ASSERT(table->no_keyread ||
              !table->covering_keys.is_set(tab->index) ||
              table->file->keyread == tab->index);
  tab->table->status=0;
  tab->read_record.read_record=join_read_prev;
  tab->read_record.table=table;
  tab->read_record.index=tab->index;
  tab->read_record.record=table->record[0];
  if (!table->file->inited)
    error= table->file->ha_index_init(tab->index, 1);
  if (!error)
    error= table->file->prepare_index_scan();
  if (error || (error= tab->table->file->ha_index_last(tab->table->record[0])))
    DBUG_RETURN(report_error(table, error));

  DBUG_RETURN(0);
}