static int rr_index_first(READ_RECORD *info)
{
  int tmp;
  // tell handler that we are doing an index scan
  if ((tmp = info->table->file->prepare_index_scan())) 
  {
    tmp= rr_handle_error(info, tmp);
    return tmp;
  }

  tmp= info->table->file->ha_index_first(info->record);
  info->read_record= rr_index;
  if (tmp)
    tmp= rr_handle_error(info, tmp);
  return tmp;
}