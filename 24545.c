int ha_maria::rnd_next(uchar *buf)
{
  register_handler(file);
  return maria_scan(file, buf);
}