static int rr_handle_error(READ_RECORD *info, int error)
{
  if (info->thd->killed)
  {
    info->thd->send_kill_message();
    return 1;
  }

  if (error == HA_ERR_END_OF_FILE)
    error= -1;
  else
  {
    if (info->print_error)
      info->table->file->print_error(error, MYF(0));
    if (error < 0)                            // Fix negative BDB errno
      error= 1;
  }
  return error;
}