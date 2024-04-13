static int imap_msg_open_new(struct Mailbox *m, struct Message *msg, const struct Email *e)
{
  int rc = -1;

  struct Buffer *tmp = mutt_buffer_pool_get();
  mutt_buffer_mktemp(tmp);

  msg->fp = mutt_file_fopen(mutt_b2s(tmp), "w");
  if (!msg->fp)
  {
    mutt_perror(mutt_b2s(tmp));
    goto cleanup;
  }

  msg->path = mutt_buffer_strdup(tmp);
  rc = 0;

cleanup:
  mutt_buffer_pool_release(&tmp);
  return rc;
}