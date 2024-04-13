int imap_hcache_del(struct ImapMboxData *mdata, unsigned int uid)
{
  if (!mdata->hcache)
    return -1;

  char key[16];

  sprintf(key, "/%u", uid);
  return mutt_hcache_delete_record(mdata->hcache, key, mutt_str_len(key));
}