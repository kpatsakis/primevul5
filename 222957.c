int imap_sync_mailbox(struct Mailbox *m, bool expunge, bool close)
{
  if (!m)
    return -1;

  struct Email **emails = NULL;
  int oldsort;
  int rc;
  int check;

  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);

  if (adata->state < IMAP_SELECTED)
  {
    mutt_debug(LL_DEBUG2, "no mailbox selected\n");
    return -1;
  }

  /* This function is only called when the calling code expects the context
   * to be changed. */
  imap_allow_reopen(m);

  check = imap_check_mailbox(m, false);
  if (check < 0)
    return check;

  /* if we are expunging anyway, we can do deleted messages very quickly... */
  if (expunge && (m->rights & MUTT_ACL_DELETE))
  {
    rc = imap_exec_msgset(m, "UID STORE", "+FLAGS.SILENT (\\Deleted)",
                          MUTT_DELETED, true, false);
    if (rc < 0)
    {
      mutt_error(_("Expunge failed"));
      return rc;
    }

    if (rc > 0)
    {
      /* mark these messages as unchanged so second pass ignores them. Done
       * here so BOGUS UW-IMAP 4.7 SILENT FLAGS updates are ignored. */
      for (int i = 0; i < m->msg_count; i++)
      {
        struct Email *e = m->emails[i];
        if (!e)
          break;
        if (e->deleted && e->changed)
          e->active = false;
      }
      if (m->verbose)
      {
        mutt_message(ngettext("Marking %d message deleted...",
                              "Marking %d messages deleted...", rc),
                     rc);
      }
    }
  }

#ifdef USE_HCACHE
  imap_hcache_open(adata, mdata);
#endif

  /* save messages with real (non-flag) changes */
  for (int i = 0; i < m->msg_count; i++)
  {
    struct Email *e = m->emails[i];
    if (!e)
      break;

    if (e->deleted)
    {
      imap_cache_del(m, e);
#ifdef USE_HCACHE
      imap_hcache_del(mdata, imap_edata_get(e)->uid);
#endif
    }

    if (e->active && e->changed)
    {
#ifdef USE_HCACHE
      imap_hcache_put(mdata, e);
#endif
      /* if the message has been rethreaded or attachments have been deleted
       * we delete the message and reupload it.
       * This works better if we're expunging, of course. */
      /* TODO: why the e->env check? */
      if ((e->env && e->env->changed) || e->attach_del)
      {
        /* L10N: The plural is chosen by the last %d, i.e. the total number */
        if (m->verbose)
        {
          mutt_message(ngettext("Saving changed message... [%d/%d]",
                                "Saving changed messages... [%d/%d]", m->msg_count),
                       i + 1, m->msg_count);
        }
        bool save_append = m->append;
        m->append = true;
        mutt_save_message_ctx(e, true, false, false, m);
        m->append = save_append;
        /* TODO: why the check for e->env?  Is this possible? */
        if (e->env)
          e->env->changed = 0;
      }
    }
  }

#ifdef USE_HCACHE
  imap_hcache_close(mdata);
#endif

  /* presort here to avoid doing 10 resorts in imap_exec_msgset */
  oldsort = C_Sort;
  if (C_Sort != SORT_ORDER)
  {
    emails = m->emails;
    m->emails = mutt_mem_malloc(m->msg_count * sizeof(struct Email *));
    memcpy(m->emails, emails, m->msg_count * sizeof(struct Email *));

    C_Sort = SORT_ORDER;
    qsort(m->emails, m->msg_count, sizeof(struct Email *), mutt_get_sort_func(SORT_ORDER));
  }

  rc = sync_helper(m, MUTT_ACL_DELETE, MUTT_DELETED, "\\Deleted");
  if (rc >= 0)
    rc |= sync_helper(m, MUTT_ACL_WRITE, MUTT_FLAG, "\\Flagged");
  if (rc >= 0)
    rc |= sync_helper(m, MUTT_ACL_WRITE, MUTT_OLD, "Old");
  if (rc >= 0)
    rc |= sync_helper(m, MUTT_ACL_SEEN, MUTT_READ, "\\Seen");
  if (rc >= 0)
    rc |= sync_helper(m, MUTT_ACL_WRITE, MUTT_REPLIED, "\\Answered");

  if (oldsort != C_Sort)
  {
    C_Sort = oldsort;
    FREE(&m->emails);
    m->emails = emails;
  }

  /* Flush the queued flags if any were changed in sync_helper. */
  if (rc > 0)
    if (imap_exec(adata, NULL, IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
      rc = -1;

  if (rc < 0)
  {
    if (close)
    {
      if (mutt_yesorno(_("Error saving flags. Close anyway?"), MUTT_NO) == MUTT_YES)
      {
        adata->state = IMAP_AUTHENTICATED;
        return 0;
      }
    }
    else
      mutt_error(_("Error saving flags"));
    return -1;
  }

  /* Update local record of server state to reflect the synchronization just
   * completed.  imap_read_headers always overwrites hcache-origin flags, so
   * there is no need to mutate the hcache after flag-only changes. */
  for (int i = 0; i < m->msg_count; i++)
  {
    struct Email *e = m->emails[i];
    if (!e)
      break;
    struct ImapEmailData *edata = imap_edata_get(e);
    edata->deleted = e->deleted;
    edata->flagged = e->flagged;
    edata->old = e->old;
    edata->read = e->read;
    edata->replied = e->replied;
    e->changed = false;
  }
  m->changed = false;

  /* We must send an EXPUNGE command if we're not closing. */
  if (expunge && !close && (m->rights & MUTT_ACL_DELETE))
  {
    if (m->verbose)
      mutt_message(_("Expunging messages from server..."));
    /* Set expunge bit so we don't get spurious reopened messages */
    mdata->reopen |= IMAP_EXPUNGE_EXPECTED;
    if (imap_exec(adata, "EXPUNGE", IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
    {
      mdata->reopen &= ~IMAP_EXPUNGE_EXPECTED;
      imap_error(_("imap_sync_mailbox: EXPUNGE failed"), adata->buf);
      return -1;
    }
    mdata->reopen &= ~IMAP_EXPUNGE_EXPECTED;
  }

  if (expunge && close)
  {
    adata->closing = true;
    imap_exec(adata, "CLOSE", IMAP_CMD_QUEUE);
    adata->state = IMAP_AUTHENTICATED;
  }

  if (C_MessageCacheClean)
    imap_cache_clean(m);

  return check;
}