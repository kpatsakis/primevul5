int ha_maria::rnd_pos(uchar *buf, uchar *pos)
{
  register_handler(file);
  int error= maria_rrnd(file, buf, my_get_ptr(pos, ref_length));
  return error;
}