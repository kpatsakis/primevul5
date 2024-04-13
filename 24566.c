int ha_maria::index_last(uchar * buf)
{
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rlast(file, buf, active_index);
  return error;
}