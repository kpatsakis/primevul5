int imap_hcache_clear_uid_seqset(struct ImapMboxData *mdata)
{
  if (!mdata->hcache)
    return -1;

  return mutt_hcache_delete_record(mdata->hcache, "/UIDSEQSET", 10);
}