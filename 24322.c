static int rr_index_last(READ_RECORD *info)
{
  int tmp= info->table->file->ha_index_last(info->record);
  info->read_record= rr_index_desc;
  if (tmp)
    tmp= rr_handle_error(info, tmp);
  return tmp;
}