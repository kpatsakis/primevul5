int rr_from_pointers(READ_RECORD *info)
{
  int tmp;
  uchar *cache_pos;

  for (;;)
  {
    if (info->cache_pos == info->cache_end)
      return -1;					/* End of file */
    cache_pos= info->cache_pos;
    info->cache_pos+= info->ref_length;

    if (!(tmp= info->table->file->ha_rnd_pos(info->record,cache_pos)))
      break;

    /* The following is extremely unlikely to happen */
    if (tmp == HA_ERR_RECORD_DELETED ||
        (tmp == HA_ERR_KEY_NOT_FOUND && info->ignore_not_found_rows))
      continue;
    tmp= rr_handle_error(info, tmp);
    break;
  }
  return tmp;
}