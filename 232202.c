bool wsrep_sst_wait ()
{
  if (mysql_mutex_lock (&LOCK_wsrep_sst)) abort();
  while (!sst_complete)
  {
    WSREP_INFO("Waiting for SST to complete.");
    mysql_cond_wait (&COND_wsrep_sst, &LOCK_wsrep_sst);
  }

  if (local_seqno >= 0)
  {
    WSREP_INFO("SST complete, seqno: %lld", (long long) local_seqno);
  }
  else
  {
    WSREP_ERROR("SST failed: %d (%s)",
                int(-local_seqno), strerror(-local_seqno));
  }

  mysql_mutex_unlock (&LOCK_wsrep_sst);

  return (local_seqno >= 0);
}