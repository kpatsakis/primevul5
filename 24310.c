int _ma_update_state_lsns(MARIA_SHARE *share, LSN lsn, TrID create_trid,
                          my_bool do_sync, my_bool update_create_rename_lsn)
{
  int res;
  DBUG_ENTER("_ma_update_state_lsns");
  mysql_mutex_lock(&share->intern_lock);
  res= _ma_update_state_lsns_sub(share, lsn, create_trid, do_sync,
                                 update_create_rename_lsn);
  mysql_mutex_unlock(&share->intern_lock);
  DBUG_RETURN(res);
}