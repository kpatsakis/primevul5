static int sync_helper(struct Mailbox *m, AclFlags right, int flag, const char *name)
{
  int count = 0;
  int rc;
  char buf[1024];

  if (!m)
    return -1;

  if ((m->rights & right) == 0)
    return 0;

  if ((right == MUTT_ACL_WRITE) && !imap_has_flag(&imap_mdata_get(m)->flags, name))
    return 0;

  snprintf(buf, sizeof(buf), "+FLAGS.SILENT (%s)", name);
  rc = imap_exec_msgset(m, "UID STORE", buf, flag, true, false);
  if (rc < 0)
    return rc;
  count += rc;

  buf[0] = '-';
  rc = imap_exec_msgset(m, "UID STORE", buf, flag, true, true);
  if (rc < 0)
    return rc;
  count += rc;

  return count;
}