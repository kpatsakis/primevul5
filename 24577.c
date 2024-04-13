int ha_maria::close(void)
{
  MARIA_HA *tmp= file;
  if (!tmp)
    return 0;
  DBUG_ASSERT(file->trn == 0 || file->trn == &dummy_transaction_object);
  DBUG_ASSERT(file->trn_next == 0 && file->trn_prev == 0);
  file= 0;
  return maria_close(tmp);
}