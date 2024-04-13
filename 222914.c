int imap_rename_mailbox(struct ImapAccountData *adata, char *oldname, const char *newname)
{
  char oldmbox[1024];
  char newmbox[1024];
  int rc = 0;

  imap_munge_mbox_name(adata->unicode, oldmbox, sizeof(oldmbox), oldname);
  imap_munge_mbox_name(adata->unicode, newmbox, sizeof(newmbox), newname);

  struct Buffer *buf = mutt_buffer_pool_get();
  mutt_buffer_printf(buf, "RENAME %s %s", oldmbox, newmbox);

  if (imap_exec(adata, mutt_b2s(buf), IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
    rc = -1;

  mutt_buffer_pool_release(&buf);

  return rc;
}