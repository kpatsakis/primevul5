int ha_maria::index_first(uchar * buf)
{
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rfirst(file, buf, active_index);
  return error;
}