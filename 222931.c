int imap_subscribe(char *path, bool subscribe)
{
  struct ImapAccountData *adata = NULL;
  struct ImapMboxData *mdata = NULL;
  char buf[2048];
  struct Buffer err;

  if (imap_adata_find(path, &adata, &mdata) < 0)
    return -1;

  if (subscribe)
    mutt_message(_("Subscribing to %s..."), mdata->name);
  else
    mutt_message(_("Unsubscribing from %s..."), mdata->name);

  snprintf(buf, sizeof(buf), "%sSUBSCRIBE %s", subscribe ? "" : "UN", mdata->munge_name);

  if (imap_exec(adata, buf, IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
  {
    imap_mdata_free((void *) &mdata);
    return -1;
  }

  if (C_ImapCheckSubscribed)
  {
    char mbox[1024];
    mutt_buffer_init(&err);
    err.dsize = 256;
    err.data = mutt_mem_malloc(err.dsize);
    size_t len = snprintf(mbox, sizeof(mbox), "%smailboxes ", subscribe ? "" : "un");
    imap_quote_string(mbox + len, sizeof(mbox) - len, path, true);
    if (mutt_parse_rc_line(mbox, &err))
      mutt_debug(LL_DEBUG1, "Error adding subscribed mailbox: %s\n", err.data);
    FREE(&err.data);
  }

  if (subscribe)
    mutt_message(_("Subscribed to %s"), mdata->name);
  else
    mutt_message(_("Unsubscribed from %s"), mdata->name);
  imap_mdata_free((void *) &mdata);
  return 0;
}