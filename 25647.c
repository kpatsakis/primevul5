join_ft_read_next(READ_RECORD *info)
{
  int error;
  if ((error= info->table->file->ha_ft_read(info->table->record[0])))
    return report_error(info->table, error);
  return 0;
}