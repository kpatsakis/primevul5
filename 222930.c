int imap_exec_msgset(struct Mailbox *m, const char *pre, const char *post,
                     int flag, bool changed, bool invert)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  if (!adata || (adata->mailbox != m))
    return -1;

  struct Email **emails = NULL;
  short oldsort;
  int pos;
  int rc;
  int count = 0;

  struct Buffer cmd = mutt_buffer_make(0);

  /* We make a copy of the headers just in case resorting doesn't give
   exactly the original order (duplicate messages?), because other parts of
   the ctx are tied to the header order. This may be overkill. */
  oldsort = C_Sort;
  if (C_Sort != SORT_ORDER)
  {
    emails = m->emails;
    // We overcommit here, just in case new mail arrives whilst we're sync-ing
    m->emails = mutt_mem_malloc(m->email_max * sizeof(struct Email *));
    memcpy(m->emails, emails, m->email_max * sizeof(struct Email *));

    C_Sort = SORT_ORDER;
    qsort(m->emails, m->msg_count, sizeof(struct Email *), compare_uid);
  }

  pos = 0;

  do
  {
    mutt_buffer_reset(&cmd);
    mutt_buffer_add_printf(&cmd, "%s ", pre);
    rc = make_msg_set(m, &cmd, flag, changed, invert, &pos);
    if (rc > 0)
    {
      mutt_buffer_add_printf(&cmd, " %s", post);
      if (imap_exec(adata, cmd.data, IMAP_CMD_QUEUE) != IMAP_EXEC_SUCCESS)
      {
        rc = -1;
        goto out;
      }
      count += rc;
    }
  } while (rc > 0);

  rc = count;

out:
  mutt_buffer_dealloc(&cmd);
  if (oldsort != C_Sort)
  {
    C_Sort = oldsort;
    FREE(&m->emails);
    m->emails = emails;
  }

  return rc;
}