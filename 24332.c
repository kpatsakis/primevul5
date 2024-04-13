int rr_sequential(READ_RECORD *info)
{
  int tmp;
  while ((tmp= info->table->file->ha_rnd_next(info->record)))
  {
    /*
      rnd_next can return RECORD_DELETED for MyISAM when one thread is
      reading and another deleting without locks.
    */
    if (info->thd->killed || (tmp != HA_ERR_RECORD_DELETED))
    {
      tmp= rr_handle_error(info, tmp);
      break;
    }
  }
  return tmp;
}