static int rr_index_desc(READ_RECORD *info)
{
  int tmp= info->table->file->ha_index_prev(info->record);
  if (tmp)
    tmp= rr_handle_error(info, tmp);
  return tmp;
}