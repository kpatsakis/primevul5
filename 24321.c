bool init_read_record_idx(READ_RECORD *info, THD *thd, TABLE *table,
                          bool print_error, uint idx, bool reverse)
{
  int error= 0;
  DBUG_ENTER("init_read_record_idx");

  empty_record(table);
  bzero((char*) info,sizeof(*info));
  info->thd= thd;
  info->table= table;
  info->record= table->record[0];
  info->print_error= print_error;
  info->unlock_row= rr_unlock_row;

  table->status=0;			/* And it's always found */
  if (!table->file->inited &&
      (error= table->file->ha_index_init(idx, 1)))
  {
    if (print_error)
      table->file->print_error(error, MYF(0));
  }

  /* read_record will be changed to rr_index in rr_index_first */
  info->read_record= reverse ? rr_index_last : rr_index_first;
  DBUG_RETURN(error != 0);
}