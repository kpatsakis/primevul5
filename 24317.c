static int rr_quick(READ_RECORD *info)
{
  int tmp;
  while ((tmp= info->select->quick->get_next()))
  {
    if (info->thd->killed || (tmp != HA_ERR_RECORD_DELETED))
    {
      tmp= rr_handle_error(info, tmp);
      break;
    }
  }
  return tmp;
}