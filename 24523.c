bool ha_maria::has_active_transaction(THD *thd)
{
  return (maria_hton && THD_TRN);
}