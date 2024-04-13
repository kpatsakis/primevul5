int ha_maria::index_prev(uchar * buf)
{
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rprev(file, buf, active_index);
  return error;
}