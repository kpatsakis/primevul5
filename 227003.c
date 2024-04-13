int imap_hcache_put(struct ImapMboxData *mdata, struct Email *e)
{
  if (!mdata->hcache)
    return -1;

  char key[16];

  sprintf(key, "/%u", imap_edata_get(e)->uid);
  return mutt_hcache_store(mdata->hcache, key, mutt_str_len(key), e, mdata->uidvalidity);
}