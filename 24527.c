int ha_maria::index_next_same(uchar * buf,
                              const uchar *key __attribute__ ((unused)),
                              uint length __attribute__ ((unused)))
{
  int error;
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  /*
    TODO: Delete this loop in Maria 1.5 as versioning will ensure this never
    happens
  */
  do
  {
    error= maria_rnext_same(file,buf);
  } while (error == HA_ERR_RECORD_DELETED);
  return error;
}