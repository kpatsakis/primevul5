void wsrep_sst_continue ()
{
  if (sst_needed)
  {
    WSREP_INFO("Signalling provider to continue.");
    // local_uuid and local_seqno are global variables and are volatile
    wsrep_uuid_t  const sst_uuid  = local_uuid;
    wsrep_seqno_t const sst_seqno = local_seqno;
    wsrep_sst_received (wsrep, sst_uuid, sst_seqno, NULL, 0);
  }
}