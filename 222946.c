int imap_create_mailbox(struct ImapAccountData *adata, char *mailbox)
{
  char buf[2048], mbox[1024];

  imap_munge_mbox_name(adata->unicode, mbox, sizeof(mbox), mailbox);
  snprintf(buf, sizeof(buf), "CREATE %s", mbox);

  if (imap_exec(adata, buf, IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
  {
    mutt_error(_("CREATE failed: %s"), imap_cmd_trailer(adata));
    return -1;
  }

  return 0;
}