char *imap_hcache_get_uid_seqset(struct ImapMboxData *mdata)
{
  if (!mdata->hcache)
    return NULL;

  char *seqset = NULL;
  size_t dlen = 0;
  char *hc_seqset = mutt_hcache_fetch_raw(mdata->hcache, "/UIDSEQSET", 10, &dlen);
  if (hc_seqset)
  {
    seqset = mutt_strn_dup(hc_seqset, dlen);
    mutt_hcache_free_raw(mdata->hcache, (void **) &hc_seqset);
  }
  mutt_debug(LL_DEBUG3, "Retrieved /UIDSEQSET %s\n", NONULL(seqset));

  return seqset;
}