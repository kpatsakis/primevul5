int imap_hcache_store_uid_seqset(struct ImapMboxData *mdata)
{
  if (!mdata->hcache)
    return -1;

  /* The seqset is likely large.  Preallocate to reduce reallocs */
  struct Buffer buf = mutt_buffer_make(8192);
  imap_msn_index_to_uid_seqset(&buf, mdata);

  int rc = mutt_hcache_store_raw(mdata->hcache, "/UIDSEQSET", 10, buf.data,
                                 mutt_buffer_len(&buf) + 1);
  mutt_debug(LL_DEBUG3, "Stored /UIDSEQSET %s\n", buf.data);
  mutt_buffer_dealloc(&buf);
  return rc;
}