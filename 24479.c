int ha_maria::ft_read(uchar * buf)
{
  int error;

  if (!ft_handler)
    return -1;

  register_handler(file);

  thread_safe_increment(table->in_use->status_var.ha_read_next_count,
                        &LOCK_status);  // why ?

  error= ft_handler->please->read_next(ft_handler, (char*) buf);

  return error;
}