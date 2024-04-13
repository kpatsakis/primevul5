int ha_maria::reset(void)
{
  ma_set_index_cond_func(file, NULL, 0);
  ds_mrr.dsmrr_close();
  if (file->trn)
  {
    /* Next statement is a new statement. Ensure it's logged */
    trnman_set_flags(file->trn,
                     trnman_get_flags(file->trn) & ~TRN_STATE_INFO_LOGGED);
  }
  return maria_reset(file);
}