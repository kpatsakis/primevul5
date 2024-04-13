void imap_hcache_open(struct ImapAccountData *adata, struct ImapMboxData *mdata)
{
  if (!adata || !mdata)
    return;

  if (mdata->hcache)
    return;

  struct HeaderCache *hc = NULL;
  struct Buffer *mbox = mutt_buffer_pool_get();
  struct Buffer *cachepath = mutt_buffer_pool_get();

  imap_cachepath(adata->delim, mdata->name, mbox);

  if (strstr(mutt_buffer_string(mbox), "/../") ||
      mutt_str_equal(mutt_buffer_string(mbox), "..") ||
      mutt_strn_equal(mutt_buffer_string(mbox), "../", 3))
  {
    goto cleanup;
  }
  size_t len = mutt_buffer_len(mbox);
  if ((len > 3) && (strcmp(mutt_buffer_string(mbox) + len - 3, "/..") == 0))
    goto cleanup;

  struct Url url = { 0 };
  mutt_account_tourl(&adata->conn->account, &url);
  url.path = mbox->data;
  url_tobuffer(&url, cachepath, U_PATH);

  const char *const c_header_cache =
      cs_subset_path(NeoMutt->sub, "header_cache");
  hc = mutt_hcache_open(c_header_cache, mutt_buffer_string(cachepath), imap_hcache_namer);

cleanup:
  mutt_buffer_pool_release(&mbox);
  mutt_buffer_pool_release(&cachepath);
  mdata->hcache = hc;
}