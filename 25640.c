join_read_first(JOIN_TAB *tab)
{
  int error= 0;
  TABLE *table=tab->table;
  DBUG_ENTER("join_read_first");

  DBUG_ASSERT(table->no_keyread ||
              !table->covering_keys.is_set(tab->index) ||
              table->file->keyread == tab->index);
  tab->table->status=0;
  tab->read_record.read_record=join_read_next;
  tab->read_record.table=table;
  tab->read_record.index=tab->index;
  tab->read_record.record=table->record[0];
  if (!table->file->inited)
    error= table->file->ha_index_init(tab->index, tab->sorted);
  if (!error)
    error= table->file->prepare_index_scan();
  if (error || (error=tab->table->file->ha_index_first(tab->table->record[0])))
  {
    if (error != HA_ERR_KEY_NOT_FOUND && error != HA_ERR_END_OF_FILE)
      report_error(table, error);
    DBUG_RETURN(-1);
  }
  DBUG_RETURN(0);
}