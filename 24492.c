int ha_maria::restart_rnd_next(uchar *buf)
{
  int error;
  register_handler(file);
  if ((error= (*file->s->scan_restore_pos)(file, remember_pos)))
    return error;
  return rnd_next(buf);
}