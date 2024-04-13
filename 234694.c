join_ft_read_next(READ_RECORD *info)
{
  int error;
  if (unlikely((error= info->table->file->ha_ft_read(info->record()))))
    return report_error(info->table, error);
  return 0;
}