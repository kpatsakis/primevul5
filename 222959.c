static int imap_mbox_check_stats(struct Mailbox *m, int flags)
{
  return imap_mailbox_status(m, true);
}