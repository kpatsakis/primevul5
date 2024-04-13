static int imap_mbox_open(struct Mailbox *m)
{
  if (!m->account || !m->mdata)
    return -1;

  char buf[PATH_MAX];
  int count = 0;
  int rc;

  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);

  mutt_debug(LL_DEBUG3, "opening %s, saving %s\n", m->pathbuf.data,
             (adata->mailbox ? adata->mailbox->pathbuf.data : "(none)"));
  adata->prev_mailbox = adata->mailbox;
  adata->mailbox = m;

  /* clear mailbox status */
  adata->status = 0;
  m->rights = 0;
  mdata->new_mail_count = 0;

  if (m->verbose)
    mutt_message(_("Selecting %s..."), mdata->name);

  /* pipeline ACL test */
  if (adata->capabilities & IMAP_CAP_ACL)
  {
    snprintf(buf, sizeof(buf), "MYRIGHTS %s", mdata->munge_name);
    imap_exec(adata, buf, IMAP_CMD_QUEUE);
  }
  /* assume we have all rights if ACL is unavailable */
  else
  {
    m->rights |= MUTT_ACL_LOOKUP | MUTT_ACL_READ | MUTT_ACL_SEEN | MUTT_ACL_WRITE |
                 MUTT_ACL_INSERT | MUTT_ACL_POST | MUTT_ACL_CREATE | MUTT_ACL_DELETE;
  }

  /* pipeline the postponed count if possible */
  struct Mailbox *m_postponed = mx_mbox_find2(C_Postponed);
  struct ImapAccountData *postponed_adata = imap_adata_get(m_postponed);
  if (postponed_adata &&
      imap_account_match(&postponed_adata->conn->account, &adata->conn->account))
  {
    imap_mailbox_status(m_postponed, true);
  }

  if (C_ImapCheckSubscribed)
    imap_exec(adata, "LSUB \"\" \"*\"", IMAP_CMD_QUEUE);

  imap_mbox_select(m);

  do
  {
    char *pc = NULL;

    rc = imap_cmd_step(adata);
    if (rc != IMAP_RES_CONTINUE)
      break;

    pc = adata->buf + 2;

    /* Obtain list of available flags here, may be overridden by a
     * PERMANENTFLAGS tag in the OK response */
    if (mutt_istr_startswith(pc, "FLAGS"))
    {
      /* don't override PERMANENTFLAGS */
      if (STAILQ_EMPTY(&mdata->flags))
      {
        mutt_debug(LL_DEBUG3, "Getting mailbox FLAGS\n");
        pc = get_flags(&mdata->flags, pc);
        if (!pc)
          goto fail;
      }
    }
    /* PERMANENTFLAGS are massaged to look like FLAGS, then override FLAGS */
    else if (mutt_istr_startswith(pc, "OK [PERMANENTFLAGS"))
    {
      mutt_debug(LL_DEBUG3, "Getting mailbox PERMANENTFLAGS\n");
      /* safe to call on NULL */
      mutt_list_free(&mdata->flags);
      /* skip "OK [PERMANENT" so syntax is the same as FLAGS */
      pc += 13;
      pc = get_flags(&(mdata->flags), pc);
      if (!pc)
        goto fail;
    }
    /* save UIDVALIDITY for the header cache */
    else if (mutt_istr_startswith(pc, "OK [UIDVALIDITY"))
    {
      mutt_debug(LL_DEBUG3, "Getting mailbox UIDVALIDITY\n");
      pc += 3;
      pc = imap_next_word(pc);
      if (mutt_str_atoui(pc, &mdata->uidvalidity) < 0)
        goto fail;
    }
    else if (mutt_istr_startswith(pc, "OK [UIDNEXT"))
    {
      mutt_debug(LL_DEBUG3, "Getting mailbox UIDNEXT\n");
      pc += 3;
      pc = imap_next_word(pc);
      if (mutt_str_atoui(pc, &mdata->uid_next) < 0)
        goto fail;
    }
    else if (mutt_istr_startswith(pc, "OK [HIGHESTMODSEQ"))
    {
      mutt_debug(LL_DEBUG3, "Getting mailbox HIGHESTMODSEQ\n");
      pc += 3;
      pc = imap_next_word(pc);
      if (mutt_str_atoull(pc, &mdata->modseq) < 0)
        goto fail;
    }
    else if (mutt_istr_startswith(pc, "OK [NOMODSEQ"))
    {
      mutt_debug(LL_DEBUG3, "Mailbox has NOMODSEQ set\n");
      mdata->modseq = 0;
    }
    else
    {
      pc = imap_next_word(pc);
      if (mutt_istr_startswith(pc, "EXISTS"))
      {
        count = mdata->new_mail_count;
        mdata->new_mail_count = 0;
      }
    }
  } while (rc == IMAP_RES_CONTINUE);

  if (rc == IMAP_RES_NO)
  {
    char *s = imap_next_word(adata->buf); /* skip seq */
    s = imap_next_word(s);                /* Skip response */
    mutt_error("%s", s);
    goto fail;
  }

  if (rc != IMAP_RES_OK)
    goto fail;

  /* check for READ-ONLY notification */
  if (mutt_istr_startswith(imap_get_qualifier(adata->buf), "[READ-ONLY]") &&
      !(adata->capabilities & IMAP_CAP_ACL))
  {
    mutt_debug(LL_DEBUG2, "Mailbox is read-only\n");
    m->readonly = true;
  }

  /* dump the mailbox flags we've found */
  if (C_DebugLevel > LL_DEBUG2)
  {
    if (STAILQ_EMPTY(&mdata->flags))
      mutt_debug(LL_DEBUG3, "No folder flags found\n");
    else
    {
      struct ListNode *np = NULL;
      struct Buffer flag_buffer;
      mutt_buffer_init(&flag_buffer);
      mutt_buffer_printf(&flag_buffer, "Mailbox flags: ");
      STAILQ_FOREACH(np, &mdata->flags, entries)
      {
        mutt_buffer_add_printf(&flag_buffer, "[%s] ", np->data);
      }
      mutt_debug(LL_DEBUG3, "%s\n", flag_buffer.data);
      FREE(&flag_buffer.data);
    }
  }

  if (!((m->rights & MUTT_ACL_DELETE) || (m->rights & MUTT_ACL_SEEN) ||
        (m->rights & MUTT_ACL_WRITE) || (m->rights & MUTT_ACL_INSERT)))
  {
    m->readonly = true;
  }

  while (m->email_max < count)
    mx_alloc_memory(m);

  m->msg_count = 0;
  m->msg_unread = 0;
  m->msg_flagged = 0;
  m->msg_new = 0;
  m->msg_deleted = 0;
  m->size = 0;
  m->vcount = 0;

  if (count && (imap_read_headers(m, 1, count, true) < 0))
  {
    mutt_error(_("Error opening mailbox"));
    goto fail;
  }

  mutt_debug(LL_DEBUG2, "msg_count is %d\n", m->msg_count);
  return 0;

fail:
  if (adata->state == IMAP_SELECTED)
    adata->state = IMAP_AUTHENTICATED;
  return -1;
}