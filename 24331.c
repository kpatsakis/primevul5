static int rr_index(READ_RECORD *info)
{
  int tmp= info->table->file->ha_index_next(info->record);
  if (tmp)
    tmp= rr_handle_error(info, tmp);
  return tmp;
}