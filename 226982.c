void imap_mdata_cache_reset(struct ImapMboxData *mdata)
{
  mutt_hash_free(&mdata->uid_hash);
  imap_msn_free(&mdata->msn);
  mutt_bcache_close(&mdata->bcache);
}