join_read_prev(READ_RECORD *info)
{
  int error;
  if ((error= info->table->file->ha_index_prev(info->record)))
    return report_error(info->table, error);
  return 0;
}