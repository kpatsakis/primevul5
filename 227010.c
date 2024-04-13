void imap_hcache_close(struct ImapMboxData *mdata)
{
  if (!mdata->hcache)
    return;

  mutt_hcache_close(mdata->hcache);
  mdata->hcache = NULL;
}