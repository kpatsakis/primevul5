struct Email *imap_hcache_get(struct ImapMboxData *mdata, unsigned int uid)
{
  if (!mdata->hcache)
    return NULL;

  char key[16];

  sprintf(key, "/%u", uid);
  struct HCacheEntry hce =
      mutt_hcache_fetch(mdata->hcache, key, mutt_str_len(key), mdata->uidvalidity);
  if (!hce.email && hce.uidvalidity)
  {
    mutt_debug(LL_DEBUG3, "hcache uidvalidity mismatch: %u\n", hce.uidvalidity);
  }

  return hce.email;
}