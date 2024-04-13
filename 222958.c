int imap_path_status(const char *path, bool queue)
{
  struct Mailbox *m = mx_mbox_find2(path);

  const bool is_temp = !m;
  if (is_temp)
  {
    m = mx_path_resolve(path);
    if (!mx_mbox_ac_link(m))
    {
      mailbox_free(&m);
      return 0;
    }
  }

  int rc = imap_mailbox_status(m, queue);

  if (is_temp)
  {
    mx_ac_remove(m);
  }

  return rc;
}