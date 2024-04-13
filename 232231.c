void wsrep_sst_complete (const wsrep_uuid_t* sst_uuid,
                         wsrep_seqno_t       sst_seqno,
                         bool                needed)
{
  if (mysql_mutex_lock (&LOCK_wsrep_sst)) abort();
  if (!sst_complete)
  {
    sst_complete = true;
    sst_needed   = needed;
    local_uuid   = *sst_uuid;
    local_seqno  = sst_seqno;
    mysql_cond_signal (&COND_wsrep_sst);
  }
  else
  {
    /* This can happen when called from wsrep_synced_cb().
       At the moment there is no way to check there
       if main thread is still waiting for signal,
       so wsrep_sst_complete() is called from there
       each time wsrep_ready changes from FALSE -> TRUE.
    */
    WSREP_DEBUG("Nobody is waiting for SST.");
  }
  mysql_mutex_unlock (&LOCK_wsrep_sst);
}