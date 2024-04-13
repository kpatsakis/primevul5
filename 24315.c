static int rr_from_tempfile(READ_RECORD *info)
{
  int tmp;
  for (;;)
  {
    if (my_b_read(info->io_cache,info->ref_pos,info->ref_length))
      return -1;					/* End of file */
    if (!(tmp= info->table->file->ha_rnd_pos(info->record,info->ref_pos)))
      break;
    /* The following is extremely unlikely to happen */
    if (tmp == HA_ERR_RECORD_DELETED ||
        (tmp == HA_ERR_KEY_NOT_FOUND && info->ignore_not_found_rows))
      continue;
    tmp= rr_handle_error(info, tmp);
    break;
  }
  return tmp;
} /* rr_from_tempfile */