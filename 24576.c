int ha_maria::index_next(uchar * buf)
{
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rnext(file, buf, active_index);
  return error;
}